#include "rpc_provider.h"
#include "rpc_header.pb.h"
#include "config.h"
#include "zookeeper_util.h"
#include "log.h"
#include <muduo/net/TcpServer.h>

namespace mrpc {


// 外部用户使用，用来注册服务
// google::protobuf::Service* service 是所有服务的基类
void RPCProvider::NotifyService( google::protobuf::Service* service ) {
    ServiceInfo service_info;

    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* p_service_descriptor = service->GetDescriptor();
    std::string service_name = p_service_descriptor->name();
    int num_method = p_service_descriptor->method_count();

    // 获取service的所有方法
    for( int i = 0; i < num_method; i++ ) {
        const google::protobuf::MethodDescriptor* p_method_descriptor = p_service_descriptor->method(i);
        std::string method_name = p_method_descriptor->name();
        service_info.m_method_map[ method_name ] = p_method_descriptor;
        DEBUG( "method name is {}:{}", service_name, method_name );
    }

    service_info.m_service = service;
    
    // 注册服务
    m_service_map[ service_name ] = service_info;
}



// 循环等待，提供远程调用服务
void RPCProvider::Run() {

    // 设置 ip 端口
    std::string rpc_ip = Config::Instance().Load( "rpc_ip" );
    uint16_t rpc_port = atoi( Config::Instance().Load( "rpc_port" ).c_str() );
    muduo::net::InetAddress addr{ rpc_ip, rpc_port };

    muduo::net::TcpServer server( &m_event_loop, addr, "RPCProvider" );

    // 绑定回调
    server.setConnectionCallback( std::bind( &RPCProvider::OnConnection, this, std::placeholders::_1 ) );
    server.setMessageCallback(std::bind(&RPCProvider::OnMessage, 
                                        this, std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3 )  );

    // 设置线程数
    int num_thread = atoi(  mrpc::Config::Instance().Load( "num_thread" ).c_str() );
    server.setThreadNum( num_thread );

    // 将当前所有的RPC服务都发布到zookeeper中
    ZKClient zclient; 
    zclient.Start();
    // service 是永久节点， method 是临时节点
    for( auto& [ service_name, service ] : m_service_map ) {
        std::string service_path = "/" + service_name; 
        zclient.Create( service_path.c_str(), nullptr, 0, 0 ); // 服务节点是永久节点
        for( auto& [ method_name, method ] : service.m_method_map ) {
            std::string method_path = service_path + "/" + method_name;  // 存储真正调用的方法的ip:port
            char method_data[128];
            sprintf( method_data, "%s:%d", rpc_ip.c_str(), rpc_port );

            INFO( "register method : {} -- {}",method_path,method_data  );
            zclient.Create( method_path.c_str(), method_data, strlen( method_data ), ZOO_EPHEMERAL ); // 建立一个临时节点
        }
    }


    INFO( "RPC Server {} start.", server.name() );
    server.start();
    m_event_loop.loop();

}

// 连接到来或者断开时调用的回调
void RPCProvider::OnConnection( const muduo::net::TcpConnectionPtr& conn ){
    if( !conn->connected() ) {
        INFO( "conn {} end.", conn->name() );
        conn->shutdown();
    }
} 

// 读事件到来时触发的回调, 有RPC请求，就及时处理并响应
// 有请求来，封装request，得到response，再发回去
void RPCProvider::OnMessage( const muduo::net::TcpConnectionPtr& conn,
                muduo::net::Buffer* buf, muduo::Timestamp ts ) 
{
    
    //  读取前四个字节内容：rpc 头部大小
    int32_t header_size = buf->readInt32();
    
    // 反序列化
    std::string rpc_head_str = buf->retrieveAsString( header_size );
    std::string service_name, method_name;
    int32_t args_size; 
    RpcHeader rpc_header; 
    
    if( rpc_header.ParseFromString( rpc_head_str ) ) {
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    }
    else {
        CRITICAL( "{} 序列化失败", rpc_head_str );
    }

    // 获取参数args, 目前只支持单参数
    std::string args_str = buf->retrieveAsString( args_size );

    // 打印信息
    INFO( "=======================================" );
    INFO( "header_size:{}", header_size );
    INFO( "rpc_head_str:{}", rpc_head_str );
    INFO( "service_name:{}", service_name );
    INFO( "method_name:{}", method_name );
    INFO( "args_size:{}", args_size );
    INFO( "args_str:{}", args_str );
    INFO( "=======================================" );

    // 调用方法
    auto it = m_service_map.find( service_name );
    if( it == m_service_map.end() ) {
        CRITICAL( "service_name is not exist!" );
    }

    auto method_it = it->second.m_method_map.find( method_name );
    if( method_it == it->second.m_method_map.end() ) {
        CRITICAL( "method_name is not exist!" );
    }

    google::protobuf::Service* service = it->second.m_service; // 被调用的对象, service类要作为this指针，不能用const
    const google::protobuf::MethodDescriptor* method = method_it->second; // 被调用的函数

    // 封装请求 和 应答
    google::protobuf::Message* request = service->GetRequestPrototype( method ).New();
    if( !request->ParseFromString( args_str ) ) {
        CRITICAL( "request args_str parse failed!, content = {}.", args_str );
    }

    google::protobuf::Message* response = service->GetResponsePrototype( method ).New();

    // 绑定回调，消息最后会通过调用该回调，被传输
    // FIXME done 有没有参数推导的能力
    // 这里没懂为什么要这么做
    google::protobuf::Closure* done = google::protobuf::NewCallback<RPCProvider,
                                                                    const muduo::net::TcpConnectionPtr&,
                                                                    google::protobuf::Message*> 
                                                                    (this, 
                                                                    &RPCProvider::SendResponse,
                                                                    conn, 
                                                                    response );        

    // 调用方法
    service->CallMethod( method, nullptr, request, response, done );         
} 

// Closure 的回调操作，用于序列化RPC和网络传输（发送）
void RPCProvider::SendResponse( const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response ) {
    // 将response 序列化后传输回去
    std::string response_str;
    if( !response->SerializeToString( &response_str ) ) {
        CRITICAL( "SerializeToString failed, {}.",response_str );
    }

    // response->PrintDebugString();
    conn->send( response_str );
    conn->shutdown(); // 模拟短连接，提供一次服务后就主动断开连接 
}





} // mrpc
