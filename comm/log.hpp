#pragma once

#include <iostream>
#include <string>

#include "util.hpp"


namespace ns_log{
    using namespace ns_util;    
    enum{
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };
    
    //开发式日志:[level][file][line]+ 其他信息
    inline std::ostream& Log(const std::string& level,const std::string& file_name,int line)
    {
        std::string message = "[";
        message += level;
        message += "]";
        
        message += "[";
        message += file_name;
        message += "]";

        message += "[";
        message += std::to_string(line);
        message += "]"; 

        message += "[";
        message += std::to_string(TimeUtil::GetTimeStamp());
        message += "]"; 
        
        std::cout << message;
        return std::cout;
    }
    //在预处理中，#是字符串化操作符，可以直接将宏参数转换为字符串字面量
    //__FILE__宏，在编译时直接替换为文件名
    //__LINE__宏，在编译时直接替换为代码行数
    #define LOG(level) Log(#level,__FILE__,__LINE__)
}