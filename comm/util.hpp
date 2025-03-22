#pragma once

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <atomic>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>


namespace ns_util{
    static const std::string Path = "./temp/";
    class PathUtil{
    public:
        static std::string AddSuffix(const std::string& file_name,const std::string& suffix)
        {
            std::string res = Path;
            res += file_name;
            res += suffix;
            return res;
        }
        static std::string Src(const std::string& file_name)
        {
            return AddSuffix(file_name,".cpp");
        }
        static std::string Exec(const std::string& file_name)
        {
            return AddSuffix(file_name,".exe");
        }
        static std::string CompileError(const std::string& file_name)
        {
            return AddSuffix(file_name,".compile_err");
        }
        static std::string Stdin(const std::string& file_name)
        {
            return AddSuffix(file_name,".stdin");
        }
        static std::string Stdout(const std::string& file_name)
        {
            return AddSuffix(file_name,".stdout");
        }
        static std::string Stderr(const std::string& file_name)
        {
            return AddSuffix(file_name,".stderr");
        }
    };
    

    class TimeUtil{
    public:
        static int GetTimeStamp()
        {
            struct timeval tv;
            gettimeofday(&tv,nullptr);
            return tv.tv_sec;
        }
        static int GetMsStamp()
        {
            struct timeval tv;
            gettimeofday(&tv,nullptr);
            return tv.tv_sec*1000 + tv.tv_usec/1000;
        }
    };
    class FileUtil{
    public:
        static bool IsExistPathName(const std::string& path_name)
        {
            struct stat st;
            int res = stat(path_name.c_str(),&st);
            return res == 0;
        } 
        static std::string GetUniqeFileName()
        {
            static std::atomic<size_t> id(0);
            id++;
            int ms = TimeUtil::GetMsStamp();
            std::string res;
            res += std::to_string(ms);
            res += '_';
            res += std::to_string(id);
            return res;
        }
        static bool WriteToFile(const std::string& target,const std::string& message)
        {
            std::ofstream out(target);
            if(!out.is_open())
            {
                return false;
            }
            out << message;
            out.close();
            return true;
        }
        static bool ReadFromFile(const std::string& target,std::string* message,bool ctrl = true)
        {
            if(!message)
                return false;
            std::ifstream in(target);
            if(!in.is_open())
                return false;
            std::string tmp;
            while(std::getline(in,tmp))
            {
                *message += tmp;
                if(ctrl)
                {
                    *message += '\n';
                }
            }
            return true;
        }
    };

    class StringUtil{
    public:
        static bool SplitString(const std::string& str,std::vector<std::string>* out,const std::string& sep)
        {
            if(!out) return false;
            out->clear();
            boost::algorithm::split(*out,str,boost::is_any_of(sep),boost::token_compress_on);        
            return true;
        }
    };
}