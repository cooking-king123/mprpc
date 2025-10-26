#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

int main(int argc, char **argv)
{
    // 整个程序启动之后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel);
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123456");
    // rpc的响应方法
    fixbug::LoginResponse response;
    // 发起rpc方法的调用，同步RPC调用过程 MPrpcChannel::callmethod
    MprpcController controller;
    stub.Login(&controller, &request, &response, nullptr); // RpcChannel->RpcChannel::callMethod集中来做所有的rpc方法调用的参数序列化和网络发送

    // 一次rpc调用完成，读调用结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc login response succes:" << response.sucess() << std::endl;
        }
        else
        {
            std::cout << "rpc login response erroe:" << response.result().errcode() << std::endl;
        }
    }

    return 0;
}