#include <iostream>

#include "../comm/httplib.h"

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

    //路由功能
    svr.Get("/all_questions", [](const Request& req,Response& resp){
        //获取所有题目
        std::string html;
        
        html = "这是所有的题目";

        resp.set_content(html,"text/html;charset=utf-8");
    });

    svr.Get(R"(/one_question/(\d+))", [](const Request& req,Response& resp){
        
    });

    svr.Post(R"(/judge/(\d+))", [](const Request& req,Response& resp){
        
    });
    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", atoi(argv[1]));


    return 0;
}