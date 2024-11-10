#pragma once

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"


namespace ns_runner{
    using namespace ns_log;
    using namespace ns_util;

    //只考虑程序能否运行成功，不考虑结果
    class Runner{
    public:
        /*****
         * 程序运行情况：
         * 1.系统自身发生错误---返回负数
         * 2.程序被信号杀掉了---返回信号
         * 3.程序运行成功--- 返回0
        *******/

        static void SetProcLimit(int _cpu_limit,int _mem_limit)
        {
            struct rlimit cpu_limit;
            cpu_limit.rlim_cur = _cpu_limit;
            cpu_limit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &cpu_limit);

            struct rlimit mem_limit;
            mem_limit.rlim_max = RLIM_INFINITY;
            mem_limit.rlim_cur = _mem_limit*1024;
            setrlimit(RLIMIT_AS,&mem_limit);
        }
        //memory单位：kb
        static int Run(const std::string& file_name,int cpu,int memory)
        {
            std::string _exe_name = PathUtil::Exec(file_name);
            std::string _stdin_name = PathUtil::Stdin(file_name);
            std::string _stdout_name = PathUtil::Stdout(file_name);
            std::string _stderr_name = PathUtil::Stderr(file_name);
            
            umask(0);
            
            int _stdin_fd = open(_stdin_name.c_str(),O_CREAT | O_RDONLY,0644);
            int _stdout_fd = open(_stdout_name.c_str(),O_CREAT | O_WRONLY ,0644);
            int _stderr_fd = open(_stderr_name.c_str(),O_CREAT | O_WRONLY, 0644);
            if(_stdin_fd == -1 || _stdout_fd == -1 || _stderr_fd == -1)
            {
                LOG(ERROR) << "打开标准文件错误" << std::endl;
                return -1; //代表打开文件失败
            }

            pid_t pid = fork();
            if(pid < 0)
            {
                LOG(ERROR) << "创建子进程失败" << std::endl;
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2;
            }
            if(pid == 0)
            {
                //子进程
                dup2(_stdin_fd,0);
                dup2(_stdout_fd,1);
                dup2(_stderr_fd,2);
                
                SetProcLimit(cpu,memory);

                execl(_exe_name.c_str(),_exe_name.c_str(),nullptr);
                

                exit(1);
            }
            else{
                //父进程
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int st;
                //程序运行异常一定是收到了信号
                waitpid(pid,&st,0);
                LOG(INFO) << "运行完毕, info: " << (st & 0x7F) << "\n"; 
                return 0x7f & st;
            }
        }
    };
}

