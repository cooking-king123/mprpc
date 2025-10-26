#include "mprpcconfig.h"
#include <iostream>
 //负责解析加载配置文件
    void MprpcConfig::LoadConfigFile(const char* config_file)
    {
        FILE *pf=fopen(config_file,"r");//读取配置文件
        if(nullptr==pf)
        {
            std::cout<<config_file<<" is note exits!"<<std::endl;
            exit(EXIT_FAILURE);
        }
        //1.注释 2.争取的配置项  3.去掉开头多余的空格
        while(!feof(pf))
        {
             char buf[512]={0};
             fgets(buf,512,pf);

             //去掉字符串前面多余的空格
             std::string read_buf(buf);
             Trim(read_buf);
             //判断#的注释
             if(read_buf[0]=='#'||read_buf.empty())
             {
                continue;
             }

             //解析配置项
             int idex=read_buf.find('=');
             if(idex==-1)
             {
                //配置项不合法
                continue;
             }
             std::string key;
             std::string value;
             key=read_buf.substr(0,idex);
             Trim(key);
             //rpcserverip=127.0.0.1\n
             int endidex=read_buf.find('\n',idex);
             value=read_buf.substr(idex+1,endidex-idex-1);
             Trim(value);
             m_configMap.insert({key,value});
        }

    }
    //查询配置信息
    std::string MprpcConfig::Load(const std::string &key)
    {
        auto it= m_configMap.find(key);
        if(it==m_configMap.end())
        {
            return "";
        }
        return it->second; 
   }

    //去掉字符串前后的空格`
    void MprpcConfig::Trim(std::string &src_buf)
    {
       
             int idex=src_buf.find_first_not_of(' ');
             if(idex!=-1)
             {
                //说明有空格
                src_buf=src_buf.substr(idex,src_buf.size()-idex);
             }
             //去掉字符串后面多余的空格、
             idex=src_buf.find_last_not_of(' ');
             if(idex!=-1)
             {
                src_buf=src_buf.substr(0,idex+1);
             }
    }

