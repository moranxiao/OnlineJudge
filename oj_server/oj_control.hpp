#pragma once

#include <mutex>
#include <vector>
#include <iostream>
#include <fstream>

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "../comm/httplib.h"
#include "../comm/util.hpp"

namespace ns_control{
    using namespace ns_model;
    using namespace ns_view;
    using namespace ns_util;
    struct Machine{
        std::string _ip;
        int _port;
        size_t _load;
        std::mutex* _mtx;

        Machine(const std::string& ip,int port)
        :_ip(ip),
        _port(port),
        _load(0),
        _mtx(new std::mutex)
        {}

        void UpLoad()
        {
            _mtx->lock();
            _load++;
            _mtx->unlock();
        }
        void DownLoad()
        {
            _mtx->lock();
            _load--;
            _mtx->unlock();
        }

        size_t Load()
        {
            size_t load;
            _mtx->lock();
            load = _load;
            _mtx->unlock();
            return load;
        }
        ~Machine()
        {
            if(_mtx) delete _mtx;
        }
    };

    const std::string machines_conf = "./cnf/service_machine.conf";

    class LoadBlance{
        bool LoginMachines()
        {
            std::ifstream in(machines_conf);
            if(!in.is_open())
            {
                return false;
            }

            std::string line;
            while(getline(in,line))
            {
                std::vector<std::string> tokens;
                StringUtil::SplitString(line,&tokens,":");
                if(tokens.size() != 2)
                {
                    LOG(WARNING) << "某个判题服务器配置出错" << "\n";
                }
                Machine mac(tokens[0],stoi(tokens[1]));
                _onlines.push_back(_machines.size());
                _machines.push_back(mac);
            }
            in.close();
            return true;
        }
    public:
        LoadBlance()
        {
            if(!LoginMachines())
            {
                LOG(FATAL) << "未能读取判题服务器配置文件" << "\n";
            }
            else
            {
                LOG(INFO) << "加载主机成功" << "\n";
            }
        }
        
        bool OnlineMachine()
        {
            
        }
        bool OfflineMachine()
        {

        }
        bool SmartChoice()
        {
            if(_onlines.size() == 0) return false;
            
        }
    private:
        std::vector<Machine> _machines;
        std::vector<int> _onlines;
        std::vector<int> _offlines;
        std::mutex _mtx;
    };

    class Control{
    public:
        bool AllQuestions(std::string* html)
        {
            if(!html) return false;
            
            std::vector<Question> questions;
            if(!_model.GetAllQuestions(&questions))
            {
                LOG(ERROR) << "用户读取所有题目失败" << '\n';
                return false;
            }
            
            _view.ShowAllQuestion(questions,html);
            
            return true;
        }
        bool OneQuestion(const std::string& id,std::string* html)
        {
            if(!html) return false;
            Question quest;
            if(!_model.GetOneQuestion(id,&quest))
            {
                LOG(WARNING) << "用户读取题目[" << id << "]失败" << '\n';
                return false;
            }
            _view.ShowOneQuestion(quest,html);
            return true;
        }
        void Judge(const std::string& id,const std::string& in_json,std::string* out_json)
        {
            if(!out_json) return;
            //先得到此题信息
            Question quest;
            if(!_model.GetOneQuestion(id,&quest)) return;
            
            Json::Reader reader;
            Json::Value root;
            reader.parse(in_json,root);
            std::string prev_code = root["code"].asString();
            std::string input = root["input"].asString();

            //构建编译运行的json串
            Json::Value compile_root;
            compile_root["code"] = prev_code + quest._test_code;
            compile_root["input"] = input;
            compile_root["cup_limit"] = quest._cpu_limit;
            compile_root["mem_limit"] = quest._mem_limit;

            //负载均衡的选择主机进行判题任务


            
        }
    private:
        Model _model;
        View _view;
    };
}