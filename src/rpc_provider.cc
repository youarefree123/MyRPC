#include "rpc_provider.h"
#include "log.h"
#include "config.h"

namespace mrpc {


// 外部用户使用，用来注册服务
// google::protobuf::Service* service 是所有服务的基类
void RPCProvider::NotifyService( google::protobuf::Service* service ) {
    ServiceInfo service_info;

    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* p_service_descriptor = service->GetDescriptor();
    std::string service_name = p_service_descriptor->name();
    int num_method = p_service_descriptor->method_count();

    DEBUG( "service name is {}", service_name );

    // 获取service的所有方法
    for( int i = 0; i < num_method; i++ ) {
        const google::protobuf::MethodDescriptor* p_method_descriptor = p_service_descriptor->method(i);
        std::string method_name = p_method_descriptor->name();
        service_info.m_method_map[ method_name ] = p_method_descriptor;
        DEBUG( "    service name is {}", method_name );
    }

    service_info.m_service = p_service_descriptor;
    
    // 注册服务
    m_service_map[ service_name ] = service_info;
}



// 循环等待，提供远程调用服务
void Run() {

    auto check = [&]( const char* key ) -> std::string {
        std::optional<std::string> opt = Config::Instance().Load( key );
        if( !opt ) {
            CRITICAL( "{} is Not Config.", key );
        }
        return opt.value();
    };

    // ip 端口
    std::string rpc_ip = check( "rpc_ip" );
    uint16_t rpc_port = atoi( check( "rpc_port" ).c_str() );
    muduo::net::InetAddress addr{ ip, port };

}


void RPCProvider::OnConnction( const muduo::net::TcpConnectionPtr& ){} // 连接到来或者断开时调用的回调
void RPCProvider::OnMessage( const muduo::net::TcpConnectionPtr&,
                muduo::net::Buffer*, 
                muduo::Timestamp ){} // 读事件到来时触发的回调

// Closure 的回调操作，用于序列化RPC和网络传输（发送）
void RPCProvider::SendRPCResponse( const muduo::net::TcpConnectionPtr&,
                          google::protobuf::Message* ){}





} // mrpc
