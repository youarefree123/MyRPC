#include "rpc_channel.h"
#include "rpc_header.pb.h"
#include "config.h"
#include <iostream>
#include <string>
#include <muduo/net/Buffer.h>
#include <muduo/net/InetAddress.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>


namespace mrpc {


// 客户端调用，序列化请求，然后传输，然后得到序列化的结果，最终反序列化得到response 
void MRpcChannel::CallMethod(   const google::protobuf::MethodDescriptor* method,
                                google::protobuf::RpcController* controller, 
                                const google::protobuf::Message* request,
                                google::protobuf::Message* response, 
                                google::protobuf::Closure* done) 
{

    const google::protobuf::ServiceDescriptor* service = method->service();
    std::string service_name = service->name();
    std::string method_name = method->name();

    // 序列化请求参数
    int32_t args_size = 0, header_size = 0;
    std::string args_str;
    
    if( !request->SerializeToString( &args_str ) ) {
        // FIXME 使用 controller
        std::cout<<" request serialize failed.\n ";
        exit(1);
    }

    args_size = args_str.size();
    RpcHeader header; 
    header.set_service_name( service_name );
    header.set_method_name( method_name );
    header.set_args_size( args_size );

    std::string header_str;

    if( !header.SerializeToString( &header_str ) ) {
        // FIXME 使用 controller
        std::cout<<" header serialize failed.\n ";
        exit(1);
    }

    header_size = header_str.size();

    // 单次连接，直接使用socket，后期再换

    // 使用muduo::net::buffe，存储完整序列
    muduo::net::Buffer buf; 
    buf.appendInt32( header_size );
    buf.append( header_str.data(), header_size );
    // buf.appendInt32( args_size );
    buf.append( args_str.data(), args_size ); 


    
    // 打印信息
    // std::cout<<( "=======================================\n" );
    // std::cout<<"header_size:"<<header_size<<std::endl;
    // std::cout<<"header_str:"<<header_str<<std::endl;
    // std::cout<<"service_name:"<<service_name<<std::endl;
    // std::cout<<"method_name:"<<method_name<<std::endl;
    // std::cout<<"args_size:"<<args_size<<std::endl;
    // std::cout<<"args_str:"<<args_str<<std::endl;
    // std::cout<<( "=======================================\n" );
    


    auto SafeLoad = [&]( const char* key ) -> std::string {
        std::optional<std::string> opt = Config::Instance().Load( key );
        if( !opt ) {
            std::cout<<key<<"  is Not Config.\n";
            exit(1);
        }
        return opt.value();
    };

    std::string rpc_ip = SafeLoad( "rpc_ip" );
    uint16_t rpc_port = atoi( SafeLoad( "rpc_port" ).c_str() );
    

    // FIXME ： 先本地找服务，后面再加服务发现
    muduo::net::InetAddress addr{ rpc_ip, rpc_port };
    // muduo::net::TcpClient client( &loop, &addr, "rpc_client" );
    

    // 和Rpc服务端建立连接
    int clientfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( clientfd < 0 ) {
        std::cout<<" socket() error.\n ";
        exit(1);
    }
    if( connect( clientfd, addr.getSockAddr(), sizeof( sockaddr ) ) < 0 ) {
        std::cout<<" connect() error.\n ";
        close( clientfd );
        exit(1);
    } 

    // 发送request ， 接收 response
    if( send( clientfd, buf.peek(), buf.readableBytes() , 0 ) < 0 ) {
        std::cout<<" send() error.\n ";
        exit(1);
    } 

    char recv_buf[1024];
    int32_t recv_size = recv( clientfd, &recv_buf, 1024, 0 );
    if( recv_size < 0 ) {
        std::cout<<" recv() error.\n ";
        exit(1);
    }

    close( clientfd ); // 模拟短连接，单次结束后就关闭

    // 反序列化, 这里不能用string构造函数，string构造函数遇见\0直接结束了，导致接收不完全
    // protobuf 提供了直接从数组构建的方法
    // std::string response_str = std::string( recv_buf, 0, recv_size );
    
    if( response->ParseFromArray( &recv_buf, recv_size ) < 0 ) {
        std::cout<<" ParseFromString() error.\n ";
        exit(1);
    }
    // response->PrintDebugString();


}





} // mrpc