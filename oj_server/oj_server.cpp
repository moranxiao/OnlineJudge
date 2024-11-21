#include <iostream>
#include <jsoncpp/json/json.h>

#include "../comm/httplib.h"
#include "oj_control.hpp"

using namespace httplib;

void Usage(const std::string& proc)
{
    std::cout << "Usage:" << proc << ' ' << "port" << std::endl;
}

using namespace ns_control;

Control* ptr_ctrl = nullptr;

void Recovery(int signo)
{
    ptr_ctrl->Recovery();    
}

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    //设置某个信号，当对oj_server发出这个信号时，负载均衡将重启所有主机
    signal(SIGQUIT, Recovery);
    Server svr;

    Control ctrl;

    ptr_ctrl = &ctrl;
    //路由功能
    svr.Get("/all_questions", [&ctrl](const Request& req,Response& resp){
        //获取所有题目
        std::string html;
        
        ctrl.AllQuestions(&html);

        resp.set_content(html,"text/html;charset=utf-8");
    });

    svr.Get(R"(/question/(\d+))", [&ctrl](const Request& req,Response& resp){
        std::string html;
        std::string id = req.matches[1];
        ctrl.OneQuestion(id,&html);

        resp.set_content(html,"text/html;charset=utf-8");
    });

    svr.Post(R"(/judge/(\d+))", [&ctrl](const Request& req,Response& resp){
        std::string id = req.matches[1];
        std::string in_json = req.body;
        std::string out_json;
        ctrl.Judge(id,in_json,&out_json);
        resp.set_content(out_json,"application/json;charset=utf-8");
    });
    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", atoi(argv[1]));


    return 0;
}