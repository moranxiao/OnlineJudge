#pragma once

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler
{
    using namespace ns_util;
    using namespace ns_log;

    class Compiler{
    public:
        //file_name: 不包含文件后缀和路径，只是文件名
        static bool Compile(const std::string& file_name)
        {
            pid_t pid = fork();
            if(pid < 0)
            {
                LOG(ERROR) << "创建子进程失败" << std::endl;
                return false;
            }
            if(pid == 0)
            {
                //子进程
                std::string compile_err_name = PathUtil::CompileError(file_name);
                //设置权限mask，消除平台差异
                umask(0);
                //以写方式打开文件，若是新文件，权限为rw|r|r
                int file_compile_error = open(compile_err_name.c_str(),O_CREAT | O_WRONLY, 0644); 
                if(file_compile_error == -1)
                {
                    LOG(ERROR) << "打开compile_error文件失败" << std::endl;
                    exit(1);
                }
                
                //标准错误重定向到compile_err_name文件中
                //如果oldfd打开且合法，就不会出错
                dup2(file_compile_error,2);

                //子进程替换为g++,编译源文件
                execlp("g++","g++", "-o", PathUtil::Exec(file_name).c_str(),\
                PathUtil::Src(file_name).c_str(),"-D", "ONLINE_COMPILE" ,"--std=c++11", nullptr);
                
                LOG(ERROR) << "进程替换失败" << std::endl;
                exit(2);
            }
            else
            {
                //父进程
                waitpid(pid,nullptr,0);
                if( !FileUtil::IsExistPathName(PathUtil::Exec(file_name)) )
                {
                    LOG(INFO) << "代码编译错误" << std::endl;
                    return false;
                }
            }
            LOG(INFO) << "代码编译成功" << std::endl;
            return true;
        }
    };
} // namespace ns_compile
