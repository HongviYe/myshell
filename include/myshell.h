#ifndef _MYSHELL_H_
#define _MYSHELL_H_
#include <unistd.h>   //Linux系统内置头文件，包含大量针对系统调用的封装
#include <string>
#include <iostream>
#include "singleton.h"
#include <vector>
#include <map>
#include <memory>
#include "CommandFactory.h"
constexpr auto MAX_BUFF_SIZE=100;
using std::cout;
using std::endl;
using std::cin;
using std::string;

class MyShell:public Singleton<MyShell>
{

public:
    MyShell();
    void run();
    /**
     * @brief runBatch file, coomand--exec myshell batchfile.myshell arg1 arg2 ....
     * @param filename
     * @param args a vector store all the argments
     */
    void runBatch(std::string filename, std::vector<std::string> args);
    /**
     * @brief translate $var to the string store in env
     * @return the ans
     */
    string translate(string str);
public:
    std::map<string,string> env;//envirment var should be edit by any command
private:
    /**
     * @brief print name,host shell name and pwd
     */
    void print_prompt();
private:
    string input;
    string output; 
    std::vector<string> history;
    int current_history;


    std::shared_ptr<CommandFactory> cf_;

    std::streambuf *oldcin;///备份了
    std::streambuf *oldcout;
    /**
     * @brief 分割字符串，方便处理
     * 
     * @param input 
     */
    void split(std::string input);
    void setDefaultEnv();
};
#endif
