#pragma once

#include <mutex>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>

#include "oj_model2.hpp"
#include "oj_view.hpp"
#include "../comm/httplib.h"
#include "../comm/util.hpp"

namespace ns_control{
    using namespace ns_model;
    using namespace ns_view;
    using namespace ns_util;

    //这个机器类要注意，在loadblance里会进行拷贝操作，如果实现了析构函数释放锁空间会出现问题，即二次释放
    struct Machine{
        std::string _ip;
        int _port;
        uint64_t _load;
        std::mutex* _mtx;

        Machine(const std::string& ip,int port)
        :_ip(ip),
        _port(port),
        _load(0),
        _mtx(new std::mutex)
        {}

        void IncLoad()
        {
            _mtx->lock();
            _load++;
            _mtx->unlock();
        }
        void DecLoad()
        {
            _mtx->lock();
            _load--;
            _mtx->unlock();
        }

        size_t Load()
        {
            uint64_t load;
            _mtx->lock();
            load = _load;
            _mtx->unlock();
            return load;
        }
        void ResetLoad()
        {
            _mtx->lock();
            _load =0;
            _mtx->unlock();
        }

    };

    const std::string machines_conf = "./cnf/service_machine.conf";

    class LoadBlance{
        bool LoginMachines()
        {
            std::ifstream in(machines_conf);
            if(!in.is_open())
            {
                LOG(FATAL) << "未能读取判题服务器配置文件" << "\n";
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
                    continue;
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
            assert(LoginMachines());
            LOG(INFO) << "加载主机成功" << "\n";
        }

        //将下线主机全部上线策略
        void OnlineMachines()
        {
            _mtx.lock();
            _onlines.insert(_onlines.end(),_offlines.begin(),_offlines.end());
            _offlines.clear();
            _mtx.unlock();
            LOG(INFO) << "所有主机上线成功" << std::endl;
        }
        void OfflineMachine(int which)
        {
            _mtx.lock();
            std::vector<int>::iterator it = _onlines.begin();
            while(it != _onlines.end())
            {
                if(*it == which)
                {
                    _machines[which].ResetLoad();
                    _offlines.push_back(which);
                    _onlines.erase(it);
                    break;
                }
                it++;
            }
            _mtx.unlock();
        }
        bool SmartChoice(int* pnumber,Machine** ppmac)
        {
            //轮询＋hash
            _mtx.lock();
            if(_onlines.size() == 0)
            {
                _mtx.unlock();
                LOG(FATAL) << "没有在线主机，请尽快修复" << "\n";
                return false;
            }
            
            std::vector<int>::iterator it = _onlines.begin();
            int min_machine_index = 0;
            while(it != _onlines.end())
            {
                if(_machines[*it].Load() < _machines[min_machine_index].Load())
                {
                    min_machine_index = *it;
                }
                it++;
            }
            *pnumber = min_machine_index;
            *ppmac = &_machines[min_machine_index];
            _mtx.unlock();
            return true;
        }
        
        //仅仅为了调试
        void ShowMachines()
        {
             _mtx.lock();
             std::cout << "当前在线主机列表: ";
             for(auto &id : _onlines)
             {
                 std::cout << id << " ";
             }
             std::cout << std::endl;
             std::cout << "当前离线主机列表: ";
             for(auto &id : _offlines)
             {
                 std::cout << id << " ";
             }
             std::cout << std::endl;
             _mtx.unlock();
        }
    private:
        std::vector<Machine> _machines;
        std::vector<int> _onlines;
        std::vector<int> _offlines;
        std::mutex _mtx;
    };

    class Control{
    public:

        void Recovery()
        {
            _load_blance.OnlineMachines();
        }
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
        void Judge(const std::string& question_id,const std::string& in_json,std::string* out_json)
        {
            if(!out_json) return;
            //先得到此题信息
            Question quest;
            if(!_model.GetOneQuestion(question_id,&quest)) return;
            Json::Reader reader;
            Json::Value root;
            reader.parse(in_json,root);
            std::string prev_code = root["code"].asString();
            std::string input = root["input"].asString();

            //构建编译运行的json串
            Json::Value compile_root;
            //一定要加\n，如果不加会导致test_code.cpp里的条件编译和prev_code.cpp的代码连在一起，以至于无法消除条件编译
            compile_root["code"] = prev_code + "\n" +quest._test_code;
            compile_root["input"] = input;
            compile_root["cpu_limit"] = quest._cpu_limit;
            compile_root["mem_limit"] = quest._mem_limit;

            Json::StyledWriter writer;
            std::string judge_json = writer.write(compile_root);
            
            //负载均衡的选择主机进行判题任务
            int id;
            Machine* m;
            while(true)
            {
                if(!_load_blance.SmartChoice(&id,&m))
                {
                    break;
                }
                m->IncLoad();
                httplib::Client client(m->_ip,m->_port);
                LOG(INFO) << "选择主机成功,主机id: " << id << " 详情: " << m->_ip << ":" << m->_port << " 当前主机的负载是: " << m->Load() << "\n";
                if(auto res = client.Post("/compile_and_run",judge_json,"application/json;charset=utf-8"))
                {
                    if(res->status = 200)
                    {
                        *out_json = res->body;
                        LOG(INFO) << "请求编译运行服务成功" << '\n';    
                        m->DecLoad();
                        break;
                    }
                    m->DecLoad();
                }
                else
                {
                    LOG(WARNING) << "请求主机[" << id << "]" << "可能已下线" << '\n';
                    _load_blance.OfflineMachine(id);
                    _load_blance.ShowMachines();
                }
            }
            
            
        }
    private:
        Model _model;
        View _view;
        LoadBlance _load_blance;
    };
}