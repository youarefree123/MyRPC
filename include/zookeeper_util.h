#pragma once 

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>
#include <optional>

namespace mrpc { 

class ZKClient {
public:
    ZKClient();
    ~ZKClient();
    void Start(); // 启动连接zkserver

    // 简历一个znode节点路径，或者znode节点的值，默认状态是永久节点
    void Create( const char* path, const char* data, int data_size, int state = 0 );

    // 获取节点
    std::optional<std::string> GetData( const char* path );

private:
    // zk 客户端句柄
    zhandle_t* m_zhandle;

};


} // mrpc