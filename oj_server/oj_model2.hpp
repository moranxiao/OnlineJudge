#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <mysql/mysql.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_model{
    using namespace ns_log;
    using namespace ns_util;

    struct Question{
        std::string _id;
        std::string _title;
        std::string _difficulty;
        int _cpu_limit;
        int _mem_limit;
        std::string _desc;
        std::string _prev_code;
        std::string _test_code;
    };

    std::string table_name = "questions";

    class Model{
    public:
        bool QueryMysql(const std::string& sql,std::vector<Question>* questions)
        {
            
        }
        bool GetAllQuestions(std::vector<Question>* questions)
        {
            std::string sql = "select * from";
            sql += table_name;
            if(!QueryMysql(sql,questions))
                return false;
            return true;
        }

        bool GetOneQuestion(const std::string& id,Question* quest)
        {

        }
    };
}
