# 项目总体结构

1. comm : 公共模块
2. compile_server : 编译与运行模块
3. oj_server : 获取题目列表，查看题目编写题目界面，负载均衡，其他功能

# 项目发布

在OnlineJudge目录下运行：

`make`

`make output`

即可得到output目录，文件可执行文件分别为`compile_server`和`oj_server`