#include "test.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;

int main()
{
    GetFriendListsResponse rsp;
   // resultCode *c=rsp.mutable_
    user *user1=rsp.add_friend_list();
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(user::MAN);

    std::cout<<rsp.friend_list_size()<<std::endl;
    return 0;
}

int demo1()
{

    //封装了login请求对象的数据
    LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123456");

    //对象数据序列化-》char*
    std::string send_str;
    if(request.SerializeToString(&send_str))
    {
        std::cout<<send_str.c_str()<<std::endl;
    
    }

    //从send_str反序列化-》login请求对象
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str))
    {
        std::cout<<reqB.name()<<std::endl;
        std::cout<<reqB.pwd()<<std::endl;
    }
    return 0;
}