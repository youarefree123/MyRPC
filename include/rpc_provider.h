#pragma once 

#include <google/protobuf/service.h>
#include <muduo/net/EventLoop.h>

namespace mrpc {



class RPCProvider
{
private:
    /* data */
public:
    RPCProvider(/* args */) = default;
    ~RPCProvider() = default;

    // 外部用户使用，用来注册服务
    void NotifyService( google::protobuf::Service* service );

    // 循环等待，提供远程调用服务
    void Run();

private:
    muduo::net::EventLoop m_event_loop; 

    struct ServiceInfo {
        google::protobuf::Service* m_service; // 已注册的服务对象
        std::unordered_map< std::string, const google::protobuf::MethodDescriptor* > m_method_map; // 已注册服务中所有方法描述
    };

    std::unordered_map< std::string, ServiceInfo > m_service_map;  // 已注册的所有服务

    void OnConnection( const muduo::net::TcpConnectionPtr& ); // 连接到来或者断开时调用的回调
    void OnMessage( const muduo::net::TcpConnectionPtr&,
                    muduo::net::Buffer*, 
                    muduo::Timestamp ); // 读事件到来时触发的回调

    // Closure 的回调操作，用于序列化RPC和网络传输（发送）
    void SendResponse( const muduo::net::TcpConnectionPtr&,
                          google::protobuf::Message* ); 


};

} // mrpc