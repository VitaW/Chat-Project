# Chat-Project
this is a chat project，come ture function of sign\login\get_user_list\talk_one\talk_group\exit.
实现在linux red hat系统，语言C++，数据库存储MySQL，多线程实现
server分支提供了服务器端代码，通过pthread.cpp实现主子线程的创建和分配客户连接给指定子进程等操作；tcpsever.cpp主要实现tcp连接，通过libevent实现IO复用；mysql.cpp连接数据库，三个表存储用户信息：user表存所有注册用户、online表为上线用户、offline表为下线用户；controll.cpp是通过MVC，对所用户所选操作进行相应的处理和响应；其余各个cpp文件是对各个功能的实现和处理。
client分支提供客户端代码，用户通过输入操作号选择操作，得到相应的处理
