#include "test.pb.h"
#include <iostream>


using namespace std;
using namespace fixbug;

int main(int argc, char const *argv[])
{
    LoginRequest req;
    req.set_name( "zhangsan" );
    req.set_pwd("123");
    
    std::string str;
    if( req.SerializePartialToString( &str ) ) {
        cout<<"Serialize:"<<str<<endl;
    }

    LoginRequest req2;
    if( req2.ParseFromString( str ) ) {
        cout<<" Parse : ";
        cout<<req2.name()<<": "<<req2.pwd()<<endl;
    }
    
    return 0;
}
