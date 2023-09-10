#include "config.h"

int main() {
    mrpc::Config conf;
    conf.Init( "/home/jiangjun/code/MyRPC/config.ini" );
    conf.Format();
    return 0;
}