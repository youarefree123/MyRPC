#include "config.h"
#include "log.h"
#include <string>


int main() {
    LogInit(PutType::Console, LogType::INFO);
    mrpc::Config::Instance().Init( "/home/jiangjun/code/MyRPC/config.ini" );
    mrpc::Config::Instance().Format();

    std::string name =  mrpc::Config::Instance().Load( "num_thread" );
    // if( name ) {
    //     INFO( "{}", name.value() );
    // }
    // else {
    //     INFO( "num_thread not found " );
    // }

    // name =  mrpc::Config::Instance().Load( "ip" );
    // if( name ) {
    //     INFO( "{}", name.value() );
    // }
    // else {
    //     INFO( "ip not found " );
    // }

    // name =  mrpc::Config::Instance().Load( "zk_port" );
    // if( name ) {
    //     INFO( "{}", name.value() );
    // }
    // else {
    //     INFO( "zk_port not found " );
    // }

    //  auto check = [&]( const char* key ) -> std::string {
    //     std::optional<std::string> opt = mrpc::Config::Instance().Load( key );
    //     if( !opt ) {
    //         CRITICAL( "{} is Not Config.", key );
    //     }
    //     return opt.value();
    // };

    // ip 端口
    std::string rpc_ip =  mrpc::Config::Instance().Load( "rpc_ip" );
    uint16_t rpc_port = atoi(  mrpc::Config::Instance().Load( "rpc_port" ).c_str() );
    // muduo::net::InetAddress addr{ ip, port };


    rpc_ip =  mrpc::Config::Instance().Load( "ip" );

    return 0;
}