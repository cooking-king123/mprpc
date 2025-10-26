#include "rpcprovider.h"
#include <iostream>
#include <unistd.h>
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

using namespace std;

// 这里是框架提供给外部使用的，可以发布rpc方法的函数接口（知道调用的是那个方法）
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    // 获取对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    // 获取服务的对象名字
    std::string service_name = pserviceDesc->name();
    // 获取对象service方法的数量
    int methodCnt = pserviceDesc->method_count();

    std::cout << "service_name:" << service_name << std::endl;
    for (int i = 0; i < methodCnt; ++i)
    {
        // 获取服务对象指定下标的服务方法的描述（抽象描述)
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});

        std::cout << "method_name:" << method_name << std::endl;
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

// 启动rpc服务 开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventloop, address, "RpcProvider");
    // 绑定连接回调和线程读写回调--分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start service :" << ip << "port:" << port << endl;

    // 启动网络服务
    server.start();
    m_eventloop.loop();
}
// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        //断开连接
        conn->shutdown();
    }
}
/*
在框架内部，RpcProvider和RpcConsumer协商好通信间的protiburf数据类型
service_name moteod_name args
定义proto的message类型，进行数据头序列化和反序列化
service_name moteod_name args args_size

header_size(4字节)+header_str+args_str
10 "10"不把数字转成字符，不然没法知道header_size到底有多大,是多少就传多少
*/
// 已建立的用户的读写时间的回调，如果rpc服务的调用请求，那么OnMessage方法就会响应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流 Login args
    std::string recv_buf = buffer->retrieveAllAsString();

    // 数据从字符流中读取前四个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 根据header_size读取数据头的原始数据
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();

        // 获取rpc方法参数的字符里数据
        std::string args_str = recv_buf.substr(4 + header_size, args_size);
        // 调试
        std::cout << "==============================" << std::endl;
        std::cout << "Header_size:" << header_size << std::endl;
        std::cout << "rpc_header_str::" << rpc_header_str << std::endl;
        std::cout << "service_name:" << service_name << std::endl;
        std::cout << "method_name:" << method_name << std::endl;
        std::cout << "args_str:" << args_str << std::endl;
        std::cout << "==============================" << std::endl;

        // 获取service对象和method对象
        auto it = m_serviceMap.find(service_name);
        if(it == m_serviceMap.end())
        {
            std::cout << service_name << " is not exist" << std::endl;
            return;
        }

        auto mit = it->second.m_methodMap.find(method_name);
        if(mit == it->second.m_methodMap.end())
        {
            // 这个服务的方法不存在
            std::cout << service_name << ":" << method_name << " is not exit!" << std::endl;
            return;
        }

        google::protobuf::Service *service = it->second.m_service; // 获取service对象（对象
        const google::protobuf::MethodDescriptor *method = mit->second; // 获取method对象（方法

        // 生成rpc方法调用的request的响应response参数
        google::protobuf::Message *request = service->GetRequestPrototype(method).New();
        if(!request->ParseFromString(args_str))
        {
            std::cout << "request parse error, content:" << args_str << std::endl;
            return;
        }
        google::protobuf::Message *response = service->GetResponsePrototype(method).New();

        // 给下面的methmod方法的调用，绑定一个Closure的回调函数
        google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);

        // 在框架上根据远端rpc请求，调用当前节点rpc节点上发布的方法
        service->CallMethod(method, nullptr, request, response, done);
    }
    else
    {
        // 数据头反序列化失败
        std::cout << "rpc_head_str:" << rpc_header_str << "parse err!" << std::endl;
        return;
    }
}

// 回调操作，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        //序列化成功后，通过网络吧rpc方法执行的结果发送到rpc的调用方
        conn->send(response_str);
        
    }
    else{
        std::cout<<"serialize response_str error!"<<std::endl;
    }
    conn->shutdown();//模拟http短链接，由rpcprovider主动断开连接
 }