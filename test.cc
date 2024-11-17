#include <iostream>
#include <string>
#include "./comm/httplib.h"

using namespace std;
using namespace httplib;

int main()
{
    Server svr;

    svr.Get(R"(/test/(\d+))", [](const Request& req,Response& resp)
    {
        string s;
        s = req.matches[1];
        resp.set_content(s,"text/plain;charset=utf-8");
    });

    svr.listen("0.0.0.0",8080);
}