#ifndef ONLINE_COMPILE
#include "prev_code.cpp"
#endif

void test1()
{
    Solution solut;
    vector<int> nums = {12,3,99,2,8,100,34,111};
    if(solut.GetMaxNum(nums)== 111) 
    {
        cout << "通过测试用例1" << endl;
    }
    else{
        cout << "未通过测试用例1" << endl;
    }
}

void test2()
{
    Solution solut;
    vector<int> nums = {-1,0,-99,12,-888};
    if(solut.GetMaxNum(nums)== 12) 
    {
        cout << "通过测试用例2" << endl;
    }
    else{
        cout << "未通过测试用例2" << endl;
    }
}

int main()
{
    test1();
    test2();
    return 0;
}