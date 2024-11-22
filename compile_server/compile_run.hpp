#pragma once


#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"

#include <jsoncpp/json/json.h>

namespace compile_run{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;    

    class CompileAndRun{
    public:

        static void RemoveFile(const std::string& file_name)
        {
            std::string exe_path = PathUtil::Exec(file_name);
            if(FileUtil::IsExistPathName(exe_path))
                unlink(exe_path.c_str());

            std::string src_path = PathUtil::Src(file_name);
            if(FileUtil::IsExistPathName(src_path))
                unlink(src_path.c_str());

            std::string compile_err_path = PathUtil::CompileError(file_name);
            if(FileUtil::IsExistPathName(compile_err_path))
                unlink(compile_err_path.c_str());
            
            std::string stdin_path = PathUtil::Stdin(file_name);
            if(FileUtil::IsExistPathName(stdin_path))
                unlink(stdin_path.c_str());
            
            std::string stderr_path = PathUtil::Stderr(file_name);
            if(FileUtil::IsExistPathName(stderr_path))
                unlink(stderr_path.c_str());

            std::string stdout_path = PathUtil::Stdout(file_name);
            if(FileUtil::IsExistPathName(stdout_path))
                unlink(stdout_path.c_str());
        }

        static std::string GetReason(int status,const std::string& file_name)
        {
            std::string message;
            switch(status)
            {
                case 0:
                message = "运行成功！";
                break;
                case -1:
                message = "代码为空";
                break;
                case -2:
                message = "未知错误";
                break;
                case -3:
                FileUtil::ReadFromFile(PathUtil::CompileError(file_name),&message);
                break;
                case SIGFPE:
                message = "浮点数溢出";
                break;
                case SIGXCPU:
                message = "运行超时";
                break;
                case SIGABRT:
                message = "内存超过范围";
                break;
                default:
                message = "未能识别此错误：[";
                message += std::to_string(status);
                message += ']';
                break;
            }
            return message;
        }
        /*************************
         * 接受的json的格式：
         * code:代码
         * input:输入
         * cpu_limit:cpu限制 s
         * mem_limit:内存限制 kb
         * 
         * 发送的json格式：
         * 必填：
         * status:状态码
         * reason:请求结果
         * 选填：
         * stdout:程序输出结果
         * stderr:程序运行完的错误信息
        */
        static void Start(const std::string& in_json,std::string* out_json)
        {
            Json::Value in_root;
            Json::Reader read;
            read.parse(in_json,in_root);

            std::string code = in_root["code"].asString();
            std::string input = in_root["input"].asString();
            int cpu_limit = in_root["cpu_limit"].asInt();
            int mem_limit = in_root["mem_limit"].asInt();

            
            int status = 0;//运行编译的总状态码
            int run_st = 0; //程序运行返回的状态码
            
            std::string file_name;

            if(code.size() == 0)
            {
                status = -1; //代码为空
                goto END;
            }
            
            //得到一个唯一的文件名
            file_name = FileUtil::GetUniqeFileName();


            if(!FileUtil::WriteToFile(PathUtil::Src(file_name),code) 
            || !FileUtil::WriteToFile(PathUtil::Stdin(file_name),input))
            {
                status = -2; //未知错误
                goto END;
            }


            if(!Compiler::Compile(file_name))
            {
                status = -3; //编译错误
                goto END;
            }
            
            run_st = Runner::Run(file_name,cpu_limit,mem_limit);

            if(run_st < 0)
            {
                status = -2; //未知错误
            }
            else if(run_st > 0)
            {
                // 程序运行崩溃
                status = run_st;
            }
            else
            {
                //程序运行成功
                status = 0;
            }
            END:
            Json::Value out_root;
            std::string reason = GetReason(status,file_name);
            out_root["reason"] = reason;
            out_root["status"] = status;
            if(status == 0)
            {
                
                std::string stdout_mes;
                std::string stderr_mes;
                FileUtil::ReadFromFile(PathUtil::Stdout(file_name),&stdout_mes);
                FileUtil::ReadFromFile(PathUtil::Stderr(file_name),&stderr_mes);
                out_root["stdout"] = stdout_mes;
                out_root["stderr"] = stderr_mes;
            }
            Json::StyledWriter writer;
            if(out_json)
            {
                *out_json = writer.write(out_root);
            }
            RemoveFile(file_name);
        }
    };
}