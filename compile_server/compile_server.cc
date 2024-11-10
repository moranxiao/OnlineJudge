#include "compile_run.hpp"


using namespace compile_run;

int main()
{
    Json::Value test_root;
    test_root["input"] = "";
    test_root["code"] = R"(
        #include <iostream>
        int main()
        {
            std::cout << "你好 OJ" << std::endl;
            while(1)
            {
                int* a = new int;   
            }
            return 0;
        }
    )";
    test_root["cpu_limit"] = 1;
    test_root["mem_limit"] = 10240*30;
    Json::FastWriter writer; 
    std::string test_in_json = writer.write(test_root);
    std::string test_out_json;
    CompileAndRun::Start(test_in_json, &test_out_json);

    // std::cout << test_in_json << std::endl;
    // std::cout << test_out_json << std::endl;
    Json::Value out_root;
    Json::Reader reader;
    reader.parse(test_out_json,out_root);
    std::cout << out_root["reason"].asString() << std::endl;
    
    return 0;
}