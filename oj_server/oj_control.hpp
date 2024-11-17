#pragma once

#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_control{
    using namespace ns_model;

    class Control{
    public:
        bool AllQuestions(std::string* html)
        {
            
        }
        bool OneQuestion(const std::string& id,std::string* html)
        {

        }
        void Judge(const std::string& id,const std::string& in_json,std::string* out_json)
        {

        }
    private:
        Model _model;
    };
}