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
        std::string _desc;
        std::string _prev_code;
        std::string _test_code;
        int _cpu_limit;
        int _mem_limit;
    };

    std::string table_name = "题目表名";
    std::string ip = "mysql服务端ip";
    std::string user_name = "用户名";
    std::string password = "密码";
    std::string database = "数据库名字";
    uint32_t port = 3306;/*数据库端口号*/

    class Model{
    public:
        bool QueryMysql(const std::string& sql,std::vector<Question>* questions)
        {
            //创建与mysql的连接
            MYSQL* mysql = mysql_init(nullptr);

            if(!mysql_real_connect(mysql,ip.c_str(),user_name.c_str(),password.c_str(),database.c_str(),port,nullptr,0))
            {
                LOG(DEBUG) << "连接数据库失败" << "\n";
                return false;
            }
            
            mysql_set_character_set(mysql,"utf8");

            //执行sql语句
            if(mysql_query(mysql,sql.c_str()))
            {
                LOG(DEBUG) << "查询数据库失败" << "\n";
                mysql_close(mysql);
                return false;
            }

            //判断是否有结果
            MYSQL_RES* result = mysql_store_result(mysql);
            if(!result)
            {
                if (mysql_field_count(mysql) == 0) {
                    LOG(DEBUG) << "查询执行成功，但无返回结果（可能是非 SELECT 查询）" << "\n";
                } else {
                    LOG(DEBUG) << "查询数据库失败：" << mysql_error(mysql) << "\n";
                }
                mysql_close(mysql);
                return false;
            }
            
            //逐行获取结果
            MYSQL_ROW fields;
            while((fields = mysql_fetch_row(result)) != nullptr)
            {
                Question q;
                q._id = fields[0] ? fields[0] : "";
                q._title = fields[1] ? fields[1] : "";
                q._difficulty = fields[2] ? fields[2] : "";
                q._desc = fields[3] ? fields[3] : "";
                q._prev_code = fields[4] ? fields[4] : "";
                q._test_code = fields[5] ? fields[5] : "";
                q._cpu_limit = fields[6] ? atoi(fields[6]) : 0;
                q._mem_limit = fields[7] ? atoi(fields[7]) : 0;

                questions->push_back(q);
            }

            //记得关闭句柄和释放结果
            mysql_free_result(result);
            mysql_close(mysql);
            return true;
        }
        bool GetAllQuestions(std::vector<Question>* questions)
        {
            std::string sql = "select id,title,difficulty,`desc`,prev_code,test_code,cpu_limit,mem_limit from ";
            sql += table_name;
            if(!QueryMysql(sql,questions))
                return false;
            return true;
        }

        bool GetOneQuestion(const std::string& id,Question* quest)
        {
            std::string sql = "select id,title,difficulty,`desc`,prev_code,test_code,cpu_limit,mem_limit from ";
            sql += table_name;
            sql += " where id = ";
            sql += id;
            std::vector<Question> vq;
            if(!QueryMysql(sql,&vq) || vq.size() != 1)
                return false;
            *quest = vq[0];
            return true;
        }
    };
}
