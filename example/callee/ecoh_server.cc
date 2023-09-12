#include "pingpong.pb.h"
#include "rpc_provider.h"
#include "config.h"
#include "log.h"
#include <string>


class EchoServer : public mrpc::EchoService
{
public:

    std::string Echo( const std::string& msg ) {
        return msg;
    }

    std::string Hello( const std::string& name ) {
        return "Hello," + name;
    }

    // callee 调用，从请求中拿到信息，然后执行，最终封装成response，再执行done 进行网络传输
    void Echo(::google::protobuf::RpcController* controller,
                       const ::mrpc::EchoRequst* request,
                       ::mrpc::EchoResponse* response,
                       ::google::protobuf::Closure* done) 
    {
        // 解析请求
        std::string msg = request->msg();

        // 做本地业务
        std::string ret = Echo( msg );

        // 封装响应
        mrpc::ResultCode* code = response->mutable_result_code(); 
        code->set_err_code( 0 );
        code->set_err_msg( "" );
        response->set_echo_msg( ret );

        // 执行回调操作，将响应传输
        done->Run();
    }

    void Hello(::google::protobuf::RpcController* controller,
                        const ::mrpc::HelloRequst* request,
                        ::mrpc::HelloResponse* response,
                        ::google::protobuf::Closure* done)
    {
        // 解析请求
        std::string name = request->name();

        // 做本地业务
        std::string ret = Hello( name );

        // 封装响应
        mrpc::ResultCode* code = response->mutable_result_code(); 
        code->set_err_code( 0 );
        code->set_err_msg( "" );
        response->set_hello_msg( ret );

        // 执行回调操作，将响应传输
        done->Run();
    }

private :
};

int main(int argc, char const *argv[])
{
    // 初始化日志
    LogInit( PutType::Console, LogType::TRACE );
    // 初始化配置
    if( !mrpc::Config::Instance().Init( "/home/jiangjun/code/MyRPC/config.ini" ) ) {
        CRITICAL( "config init failed." );
    } 

    // Provider 代表一个RPC对象
    mrpc::RPCProvider provider;
    provider.NotifyService( new EchoServer() );

    provider.Run();
    return 0;
}



