#include "rpc_channel.h"
#include "pingpong.pb.h"
#include "config.h"

#include <iostream>


void RequestEcho( mrpc::EchoService_Stub& stub,  const std::string& msg ) {

    // 构造RPC请求
    mrpc::EchoRequst request;
    request.set_msg( msg );
    mrpc::EchoResponse response;

    // 远程过程调用,如同调用本地方法一般调用远端方法
    stub.Echo( nullptr, &request, &response, nullptr ); 

    if( response.result_code().err_code() != 0 ) {
        std::cout<<"the RPC Echo failed : "<< response.result_code().err_msg()<<std::endl;
        exit(1);
    }

    std::cout<<"The RPC Echo Success, Response: "<<response.echo_msg()<<std::endl;

}

void RequestHello( mrpc::EchoService_Stub& stub, const std::string& name ) {
    // 构造RPC请求
    mrpc::HelloRequst request;
    request.set_name( name );
    mrpc::HelloResponse response;

    // 远程过程调用,如同调用本地方法一般调用远端方法
    stub.Hello( nullptr, &request, &response, nullptr ); 

    if( response.result_code().err_code() != 0 ) {
        std::cout<<"the RPC Hello failed : "<< response.result_code().err_msg()<<std::endl;
        exit(1);
    }

    std::cout<<"The RPC Hello Success, Response: "<<response.hello_msg()<<std::endl;
}

int main() {

    if( !mrpc::Config::Instance().Init( "/home/jiangjun/code/MyRPC/config.ini" ) ) {
        std::cout<<( "config init failed." )<<std::endl;
    } 

    // 实例化服务 stub对象
    mrpc::EchoService_Stub stub( new mrpc::MRpcChannel() );
    std::string str;
    int type = 0;
    while( true ) {
        std::cin>>type;
        if( type == 0 ) {
            std::cin>>str;
            RequestEcho( stub, str );
        }
        else if( type == 1 ) {
            std::cin>>str;
            RequestHello( stub, str );
        }
        else {
            break;
        }
    }
    
    return 0;

}