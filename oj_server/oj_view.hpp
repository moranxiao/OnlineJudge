#pragma once

#include <vector>
#include <ctemplate/template.h>

#include "oj_model2.hpp"

namespace ns_view{
    using namespace ns_model;
    using namespace ctemplate;

    static const std::string html_path = "./template_html/";
    static const std::string all_questions_html = "all_questions.html";
    static const std::string one_question_html = "one_question.html";
    class View{
    public:
        void ShowAllQuestion(const std::vector<Question>& questions,std::string* html)
        {
            if(!html) return;
            
            TemplateDictionary root("all_questions");

            for(const auto& q : questions)
            {
                TemplateDictionary* row_dict = root.AddSectionDictionary("question_list");
                row_dict->SetValue("id",q._id);
                row_dict->SetValue("title",q._title);
                row_dict->SetValue("difficulty",q._difficulty);
            }
            Template* tpl = Template::GetTemplate(html_path+all_questions_html,DO_NOT_STRIP);
            tpl->Expand(html,&root);
        }
        void ShowOneQuestion(const Question& quest,std::string* html)
        {
            if(!html) return;
            
            TemplateDictionary root("one_question");

            root.SetValue("id",quest._id);
            root.SetValue("prev_code",quest._prev_code);
            root.SetValue("title",quest._title);
            root.SetValue("difficulty",quest._difficulty);
            root.SetValue("desc",quest._desc);

            Template* tpl = Template::GetTemplate(html_path+one_question_html,DO_NOT_STRIP);
            tpl->Expand(html,&root);
        }
        
    };
}