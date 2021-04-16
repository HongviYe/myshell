#include "myshell.h"
#include "CommandFactory.h"
#include "AbstractCommand.h"

#include <memory>
#include <stdio.h>
#include <termios.h>
#include <sstream>
#include <unistd.h>
#include <fstream>
int getch( ) {// linux下的getch网上的实现
    struct termios oldt,newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
/**
 * @brief 获取当前执行的可执行程序的目录
 * 
 * @return string 
 */
string GetOwnerPath()
{
    char path[1024];
    int cnt = readlink("/proc/self/exe", path, 1024);
    if(cnt < 0|| cnt >= 1024)
    {
        return NULL;
    }
    //最后一个'/' 后面是可执行程序名，去掉可执行程序的名字，只保留路径
    for(int i = cnt; i >= 0; --i)
    {
        if(path[i]=='/')
        {
            path[i + 1]='\0';
            break;
        }
    }
    string s_path(path);   //这里我为了处理方便，把char转成了string类型
    return s_path;
}
void MyShell::setDefaultEnv(){
    auto msg = (char *)malloc(100);
    getcwd(msg, 100);

    env["PWD"]=string(msg);
    env["SHELL"]=string(GetOwnerPath());
    env["UID"]=std::to_string(getuid());
    env["PATH"]="/usr/bin;/usr/sbin";
    env["HELP_FILE"]="../myshell/help.txt";
    free(msg);
}
/**
 * @brief 拷贝函数，构造一些基础环境变量，也备份了标准的输入和输出方便恢复
 * 
 */
MyShell::MyShell()
{
    setDefaultEnv();
    cf_=std::shared_ptr<CommandFactory>(new CommandFactory);
    oldcin = std::cin.rdbuf();
    oldcout=std::cout.rdbuf();
}
void MyShell::split(std::string input){

    std::stringstream ss(input);
    string key;
    string input_file;//自定义的标准输入
    string output_file;//自定义的标准输出
    std::ofstream ofile;
    std::ifstream ifile;
    string command;
    bool append=false;
    bool first=true;//用于防止最后一个变量后面有空格
    while(ss>>key){//处理重定向
        if(key=="<"){
            ss>>input_file;
        }
        else if(key==">"){
            ss>>output_file;
        }
        else if(key==">>"){
            ss>>output_file;
            append=true;//记录是否为追加模式
        }
        else{
            if(first)
                first=false;
            else
                command+=' ';
            command+=key;
        }
    }

    std::shared_ptr<AbstractCommand> ptr;

    if(command.back()=='&'){
        command.pop_back();
        ptr=cf_->getCommand(command);
        auto cp=dynamic_cast<OuterCommand*>(ptr.get());
        if(cp){
            cp->background_=true;
        }
        else{
            std::cout<<"warning,internal command should not be called by background"<<std::endl;
        }
    }
    else{
       ptr=cf_->getCommand(command);

    }
    if(!input_file.empty()){//重定向输入
        ifile.open(input_file);
        if(ifile){
           std::cin.rdbuf(ifile.rdbuf());
        }

    }
    if(!output_file.empty()){//重定向输出
        if(!append)
            ofile=std::ofstream(output_file);//非追加
        else
            ofile=std::ofstream(output_file,std::ofstream::app);//追加
        std::cout.rdbuf(ofile.rdbuf());
        ptr->setColorFlag(false);

    }
    ptr->createEnvirment();//生成环境
    if(! ptr->getOutput().empty()&&((!(ptr->getOutput()[0]==' '))))
        std::cout <<  ptr->getOutput() << std::endl;
    std::cin.rdbuf(oldcin); // 恢复键盘输入
    std::cout.rdbuf(oldcout); //恢复屏幕输出
    if(ifile)
        ifile.close();
    if(ofile)
        ofile.close();
    return ;
}
void MyShell::run()
{
    int pos;//the postion of cursor
    //从input中获取命令，调用相应函数实现，最后返回结果
    while(true)
    {
        oldcin=std::cin.rdbuf();
        oldcout=std::cout.rdbuf();
        std::ofstream ofile;
        std::ifstream ifile;

        string input_file_name;
        string output_file_name;
        string command;

        pos=0;
        print_prompt();
        input="";
        current_history=0;
        string current_input;
        while(true){//deal with every character in input stream
            int c=getch();
            switch (c) {
                case 27 ://this is the action triggered by up-arrow key
                    int d;
                    d=getch();
                    d=getch();
                    switch (d) {
                        case 68://左箭头
                            printf("\b");
                            pos--;
                        break;
                        case 65:
                            if(current_history<history.size()){
                                current_input=input;
                                while(pos){
                                    printf("\b \b");
                                    input.pop_back();
                                    pos--;
                                }
                                current_history++;
                                input=history[history.size()-current_history];
                                cout<<input;
                                pos=input.size();
                            }
                        break;
                        case 66://backspace 键
                            if(current_history>1){
                                while(pos){
                                    printf("\b \b");
                                    input.pop_back();
                                    pos--;
                                }
                                current_history--;
                                input=history[history.size()-current_history];
                                cout<<input;
                                pos=input.size();
                            }
                            else{
                                while(pos){
                                    printf("\b \b");
                                    input.pop_back();
                                    pos--;
                                }
                                input=current_input;
                                cout<<input;
                                pos=input.size();
                                current_history--;
                            }
                        break;
                        case 67:
                        break;
                    }
                break;
                case 127 :
                if(pos>0){
                    printf("\b \b");
                    input.pop_back();
                    pos--;
                }
                break;
                case (10)://Enter键
                    std::cout<<std::endl;
                break;
            default://其他的则认为是命令
                input+=char(c);
                std::cout<<char(c);
                pos++;
                break;
            }
            if(c==10){//Enter键
                history.push_back(input);
                break;
            }
        }
        if(input.empty()){
            continue;
        }
        split(input);

    }
}

void MyShell::runBatch(string filename,std::vector<string> args)
{
    std::ifstream fin(filename);
    if(!fin){//判断脚本文件是否存在
        std::cout<<"no such file!!"<<endl;
        return;
    }
    string command;

    for(int i=0;i<10;i++){//删除掉一些环境变量
        env.erase(std::to_string(i));
    }
    for(int i=0;i<args.size();i++){//新增一些环境变量如$1
        env[std::to_string(i)]=args[i];
    }
    while(getline(fin,command)){
        split(command);//分割并运行
    }
    fin.close();
}

std::string MyShell::translate(std::string str)
{
    if(str.empty()||str[0]!='$')
        return str;
    return env[str.substr(1,str.size()-1)];//
}

//命令行提示符，包含当前路径
void MyShell::print_prompt()
{
    auto msg = (char *)malloc(100);
    getcwd(msg, 100);
    cout <<"\033[32m"<< "zxd@MyShell:" <<"\033[0m"<<"\033[35m"<< string(msg) <<"\033[0m"<< '$' << " ";
    free(msg);
}
