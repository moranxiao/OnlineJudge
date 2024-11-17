#include "compile_run.hpp"
#include "../comm/httplib.h"

using namespace compile_run;

// int main()
// {
//     Json::Value test_root;
//     test_root["input"] = "";
//     test_root["code"] = R"(
//         #include <iostream>
//         int main()
//         {
//             std::cout << "你好 OJ" << std::endl;

//             std::cerr << "hehe" << std::endl;
//             return 0;
//         }
//     )";
//     test_root["cpu_limit"] = 1;
//     test_root["mem_limit"] = 10240*30;
//     Json::FastWriter writer; 
//     std::string test_in_json = writer.write(test_root);
//     std::string test_out_json;
//     CompileAndRun::Start(test_in_json, &test_out_json);

//     // std::cout << test_in_json << std::endl;
//     std::cout << test_out_json << std::endl;
//     Json::Value out_root;
//     Json::Reader reader;
//     reader.parse(test_out_json,out_root);
//     std::cout << out_root["reason"].asString() << std::endl;
    
//     return 0;
// }
using namespace httplib;

void Usage(const std::string& proc)
{
    std::cout << "Usage:" << proc << ' ' << "port" << std::endl;
}

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    Server svr;
    svr.Post("/compile_and_run",[](const Request& req,Response& resp){
        std::string in_json = req.body;
        std::string out_json;
        if(in_json.empty())
        {
            resp.body = "请求为空";
            return;
        }
        CompileAndRun::Start(in_json,&out_json);
        resp.set_content(out_json,"application/json;charset=utf-8");
    });
    
    svr.listen("0.0.0.0", atoi(argv[1]));
    return 0;
}