#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>

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

    static const std::string questions_path = "./questions/";
    static const std::string questions_conf = "questions_list.conf";
    static const std::string question_desc_name = "desc.txt";
    static const std::string question_prev_code_name = "prev_code.cpp";
    static const std::string question_test_code_name = "test_code.cpp";

    class Model{
    public:
        Model()
        {
            if(LoginQuestionsList())
            {
                LOG(INFO) << "获取所有题目成功" << std::endl;
            }
            else{
                LOG(FATAL) << "获取所有题目失败" << std::endl;
                exit(1);
            }
        }

        bool LoginQuestionsList()
        {
            std::string line;
            //打开问题列表的配置文件
            std::ifstream questions_list(questions_path+questions_conf);
            if(!questions_list.is_open())  
                return false;
            //获取列表所有问题
            std::vector<std::string> tokens;
            while(getline(questions_list,line))
            {
                std::string sep = " ";
                if(!StringUtil::SplitString(line,&tokens,sep))
                    LOG(WARNING) << "spilt string error" << "\n";
                if(tokens.size() != 5)
                {
                    LOG(WARNING) << "获取某个题目失败" << "\n";
                }           
                Question quest;
                quest._id = tokens[0];
                quest._title = tokens[1];
                quest._difficulty = tokens[2];
                quest._cpu_limit = std::stoi(tokens[3]);
                quest._mem_limit = std::stoi(tokens[4]);
                if(!FileUtil::ReadFromFile(questions_path+quest._id+"/"+question_desc_name,&quest._desc)\
                || !FileUtil::ReadFromFile(questions_path+quest._id+"/"+question_prev_code_name,&quest._prev_code)\
                || !FileUtil::ReadFromFile(questions_path+quest._id+"/"+question_test_code_name,&quest._test_code))
                    LOG(WARNING) << "获取某个题目失败,题目id:" << quest._id << "\n";
                _questions[quest._id] = quest;
            }
            questions_list.close();
            return true;
        }

        bool GetAllQuestions(std::vector<Question>* questions)
        {
            if(_questions.empty() || !questions )
            {
                LOG(ERROR) << "用户获取题库失败" << "\n";
                return false;
            }
            questions->clear();
            for(const auto& e : _questions)
            {
                questions->push_back(e.second);
            }
            return true;
        }

        bool GetOneQuestion(const std::string& id,Question* quest)
        {
            if(!quest || !_questions.count(id))
            {
                LOG(ERROR) << "用户获取题目[" << id << "]失败" << "\n";
                return false;
            }
            *quest = _questions[id];
            return true;
        }
    private:
        std::unordered_map<std::string,Question> _questions;
    };
}
