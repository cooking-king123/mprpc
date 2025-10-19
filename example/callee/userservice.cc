#include <iostream>
#include <string>
#include "user.pb.h"
using namespace fixbug;

//UserService原来是一个本地服务，提供了两个进程内的本地方法，login和FriendLists
class UserService:public UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd)//使用在rpc服务发布端（rpc服务提供者）
    {
        std::cout<<"doing local service: Login"<<std::endl;
        std::cout<<"name"<<name<<"pwd"<<pwd<<std::endl;
        return true;
    }

    //重写基类虚方法，下面这些方法都是框架直接调用的
    void Login(::google::protobuf::RpcController*  controller,
                        const ::fixbug::LoginRequest*  request,
                        ::fixbug::LoginResponse*  response,
                        ::google::protobuf::Closure*  done)
    {
        //框架给业务上报了请求参数LoginRequest，应用获取相应的数据做本地业务
        std::string name=request->name();
        std::string pwd=request->pwd();

        //做本地业务
        bool login_result=Login(name,pwd);

        //把响应写入包括错误码，错误消息，返回值
        fixbug::ResulrCode *code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_sucess(login_result);

        //执行回调操作，执行响应的对象数据的序列化和网络发送
        done->Run();


    }
};

int main()
{
    return 0;
}

