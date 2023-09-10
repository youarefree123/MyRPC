#pragma once


namespace mrpc {

// 继承自NonCopyable的类可以正常构造和析构，但是不能进程拷贝构造和拷贝赋值
class noncopyable {
public:
    noncopyable( const noncopyable& ) = delete;
    void operator=( const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;

};

} // mrpc