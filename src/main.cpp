#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "myshell.h"
/**
 * @brief myshell，运行于Ubuntu19.04，支持两种运行方式：
 * 1. 解释器，解释命令并运行
 * 2. 脚本模式，myshell file 命令可运行file脚本
 * 
 * @return int 
 */
int main(int argc,char* argv[])
{

    MyShell* ms=MyShell::GetSingletonPtr();//单体类，只能实例化一个shell
    std::vector<string> args;
    for(int i=2;i<argc;i++){
        args.push_back(string(argv[i]));
    }
    if(argc>1){
        ms->runBatch(argv[1],args);//运行
    }
    else{
        ms->run();//运行解释器模式
    }


}
