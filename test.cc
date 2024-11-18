#include <iostream>
#include <string>
// #include "./comm/httplib.h"

// using namespace std;
// using namespace httplib;

// int main()
// {
//     Server svr;

//     svr.Get(R"(/test/(\d+))", [](const Request& req,Response& resp)
//     {
//         string s;
//         s = req.matches[1];
//         resp.set_content(s,"text/plain;charset=utf-8");
//     });

//     svr.listen("0.0.0.0",8080);
// }
#include <ctemplate/template.h>

using namespace ctemplate;
using namespace std;

int main()
{
    string html_path = "./oj_server/template/";
    string html_name = "all_questions.html";
    ctemplate::TemplateDictionary root("test");
    

    for(int i = 0; i < 3; i++)
    {
        ctemplate::TemplateDictionary* question_dict = root.AddSectionDictionary("questions_num");
        question_dict->SetValue("id",std::to_string(i));
        question_dict->SetValue("titile",std::to_string(i));
        question_dict->SetValue("difficulty",std::to_string(i));

    }
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(html_path+html_name,ctemplate::DO_NOT_STRIP);
    string out_html;
    tpl->Expand(&out_html,&root);
    cout << out_html << endl;
    return 0;
}