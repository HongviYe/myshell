//抽象产品类：所有具体产品的父类，负责描述所有实例所共有的公共接口。
#include "AbstractCommand.h"
#include <dirent.h>   //定义了Linux系统目录操作的函数，主要用于文件系统的目录读取操作
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <streambuf>
#include<sys/types.h>
#include<sys/wait.h>
#include <cstring>
#include <signal.h>
#include <fstream>
#include "myshell.h"
std::vector<pid_t> AbstractCommand::suspended_processes_=std::vector<pid_t>();
pid_t AbstractCommand::tmp=0;
/*颜色字符串*/
std::map<string,string> AbstractCommand::color_map=std::map<string,string>{std::pair<string,string>("black","30"),
                                                        std::pair<string,string>("read","31"),
                                                        std::pair<string,string>("green","32"),
                                                        std::pair<string,string>("yellow","33"),
                                                        std::pair<string,string>("blue","34"),
                                                        std::pair<string,string>("purple","35"),
                                                        std::pair<string,string>("dark_green","36"),
                                                        std::pair<string,string>("white","37")};
AbstractCommand::AbstractCommand():output(""),add_color_(true),return_state_bit_(0)
{
}
/*空函数*/
void AbstractCommand::nothing(int id){}
void AbstractCommand::subprocessSuspend(int id){//pause a process
    std::cout<<std::endl<<"pause pid="<<getpid()<<" father pid="<<getppid()<<std::endl;
    //kill(getppid(),SIGINT);
    pause();
}
/**
 * @brief Ctrl+C SIGINT触发的函数
 * 
 * @param id 
 */
void AbstractCommand::Abort(int id){//ctrl + c handler function
       std::cout<<"abort"<<id;
}

void AbstractCommand::processSuspend(int id){//ctrl + \ handler function
       std::cout<<std::endl<<"stopped , please use fg or bg to restart the process"<<std::endl<<"subporcess pid="<<tmp<<std::endl<<"current pid="<<getpid()<<fflush(stdout);
       suspended_processes_.push_back(tmp);
       kill(tmp,SIGUSR2);
       //raise(SIGINT);
}


void AbstractCommand::setInput(std::string input) {
    input_line=input;
}
/**
 * @brief  预处理，做一个简单的命令分割
 *
 * @return 将分割好的字符串保存在一个数组中 
 */
std::vector<std::string> AbstractCommand::preProcess()  //初始化处理，把用户输入的命令做字符串分割
{
    std::vector<std::string> ans;
    std::stringstream ss(input_line);
    string command;
    int count=0;

    if(!key.empty())
    while(ss>>command)
    {
        if(command==key)
        {
            count++;
            continue;
        }
        else {
            if(count!=1)
            {
                output="Sytanx error! count="+char('0'+count);
                break;
            }
            ans.push_back(command);   //用于存储命令的参数个数
        }
    }
    else{
        while(ss>>command)
        {
            ans.push_back(command);   //用于存储命令的参数个数
        }
    }
    for(auto &i:ans){
        i=MyShell::GetSingletonPtr()->translate(i);
    }
    return ans;
}

std::string AbstractCommand::getpwd()
{
    auto current_pwd = (char *)malloc(100);
    getcwd(current_pwd, 100);
    auto pwd=string(current_pwd);
    free(current_pwd);
    return pwd;
}
/**
 * @brief 将任意字符串添加shell能识别的颜色
 * 
 * @param target 目标字符串
 * @param color 颜色字符串，仅支持color——map里定义的那些
 * @return 添加完颜色的字符串
 */
std::string AbstractCommand::addColor(std::string target, string color)
{
    
    if(add_color_)
        return "\033["+color_map[color]+"m"+target+"\033[0m";
    else
        return target;
}

std::string AbstractCommand::getOutput() {
    return output;
}

void AbstractCommand::setColorFlag(bool use_color)
{
    add_color_=use_color;
}



int AbstractCommand::getReturnState()
{
    return return_state_bit_;
}

void Command_CD::run()
{

    auto para_num=preProcess();
    if(para_num.size()>1)
    {
        output="Too much parameter!";
        return;
    }
    if(para_num.empty()) {   //没有参数，显示当前目录
        return;
    }
    if(para_num[0]==key)
    {
        output="Sytanx error!";
        return;
    }

    int return_value=chdir(para_num[0].c_str());   //chdir改变当前工作目录，参数为目标目录
    if(return_value) {   //目录不存在，返回错误信息
        output="No such directory!";
    }
    else{
        MyShell::GetSingletonPtr()->env["PWD"]=getpwd();//修改PWD环境变量
    }
}

void Command_Dir::run()
{
   auto dir_name=preProcess();
   string extra_dir=".";
   if(dir_name.size()>1)
   {
       output="Too much parameter!";
       return;
   }
   if(dir_name.size()==1)
        extra_dir=dir_name[0];

   string path=getpwd();
   DIR* dir_handle=opendir((path+'/'+extra_dir).c_str());  //打开目录
   if(!dir_handle) {
       output="No such directory!";
       return;
   }

   struct dirent* dir_ptr;
   bool first=true;
   while(true)
   {
       dir_ptr=readdir(dir_handle);   //读取目录文件信息，依次读出目录中的所有文件

       if(dir_ptr)
       {
           if(string(dir_ptr->d_name)!="." && string(dir_ptr->d_name)!="..")
           {
               string color;
               if((int)dir_ptr->d_type==4){
                   color="blue";
               }
               else  if((int)dir_ptr->d_type==8){
                    color="yellow";
               }
               if(first){
                   first=false;
                   output="";
               }
               else
                    output+="\n";
            output+=addColor(string(dir_ptr->d_name),color);

           }
       }
       else
           break;
   }
   closedir(dir_handle);
}

void Command_Time::run()
{
    time_t t;
    t=time(NULL);  //获取系统日期和时间
    auto l_time = localtime(&t);  //获取当地日期和时间。把函数time返回的时间转换为当地标准时间，并以tm结构形式返回
    std::stringstream ss;
    ss << l_time->tm_year+1900 << "-";//时间从1900开始
    ss << l_time->tm_mon+1 << "-";
    ss << l_time->tm_mday << " ";
    ss << l_time->tm_hour << ":";
    ss << l_time->tm_min << ":";
    ss << l_time->tm_sec;
    output=ss.str();
}

void Command_Pwd::run()
{
    output=getpwd();
}

void Command_Clr::run()
{
    system("clear");
}

void Command_Quit::run()
{
    int status;
    auto para=preProcess();
    std::stringstream ss;

    if(para.empty())//若无参数
        exit(0);

    ss << para[0];
    ss >> status;
    //output=status;
    exit(status);
}

void Command_System::run()
{
    auto p=preProcess();
    string path;
    /*这里理论上应该是遍历path的路径的，但是出于时间原因就手动写上了*/
    if(std::ifstream("/usr/bin/"+p[0])){
        path="/usr/bin/"+p[0];
    }
    else if(std::ifstream("/usr/sbin/"+p[0])){
        path="/usr/sbin/"+p[0];
    }
    else{
        output="myshell: no such command!!!";
        return;
    }
    return_state_bit_=system((input_line +" > /tmp/output").c_str());

    std::ifstream fin("/tmp/output");
    std::stringstream ss;
    ss<<fin.rdbuf();
    string ans=ss.str();
    if(ans.size()&&ans.back()=='\n')
    ans.pop_back();
    output=ans;
}

void InternalCommand::createEnvirment()
{
    run();
}

void Command_Exec::run()
{
    auto path=preProcess();
    if(path.empty()){
        output="no input!";
        return;
    }
    string command;
    for(int i=0;i<path.size();i++){
        command+=path[i];
        if(i!=path.size()-1)
            command+=" ";
    }
/*理论上这里应该是遍历path变量的*/
    if(std::ifstream(getpwd()+"/"+path[0])){
        command="./"+command;
    }
    else if(std::ifstream("/usr/bin/"+path[0])){
        command="/usr/bin/"+command;
    }
    else if(std::ifstream("/usr/sbin/"+path[0])){
        command="/usr/sbin/"+command;
    }

    if(background_)
        return_state_bit_=system((command +" > /dev/null").c_str());
    else
        return_state_bit_=system((command).c_str());

}

void Command_Exit::run()
{
    auto p=preProcess();
    if(!p.empty()){//exit不应该有参数
        output="Sytanx error!";
        return;
    }
    exit(0);
}

OuterCommand::OuterCommand()
{
    background_=false;
}

void OuterCommand::createEnvirment()
{
    int fd[2];
    pipe(fd);//Create a one-way communication channel (pipe)
    int *write_fd = &fd[1];//two handler
    int *read_fd = &fd[0];


   // signal(SIGINT,SIGTSTP);
    new_process_pid=fork();
    if(new_process_pid==0){
        signal(SIGQUIT,nothing);
        signal(SIGUSR2 ,subprocessSuspend);
        run();
        close(*read_fd);          /* Reader will see EOF */
        if(getReturnState()==0&&output.size()){
            write(*write_fd,output.c_str(),strlen(output.c_str()));
            close(*write_fd);          /* Close unused write end */
        }
        else{
            close(*write_fd);          /* Close unused write end */
            exit(-1);
        }
        exit(0);
    }
    else{
        tmp=new_process_pid;
        signal(SIGINT,Abort);
        signal(SIGQUIT,processSuspend);
        int state;
        close(*write_fd);          /* Close unused write end */
        char readbuffer[800]="";
        int count=0;
        if(!background_)
        while (read(*read_fd, readbuffer, 100) > 0){
            count++;
            output+=string(readbuffer);
        }

        close(*read_fd);

    }
}

void Command_Echo::run()
{
    auto arg=preProcess();
    for(int i=0;i<arg.size();i++){
        output+=arg[i];
        if(i!=preProcess().size()-1)
            output+=' ';
    }

}

void Command_Jobs::run()
{
    auto arg=preProcess();
    if(arg.size()){
        output="error args";
        return;
    }
    output+="number of jobs="+std::to_string(AbstractCommand::suspended_processes_.size())+'\n';
    for(int i=0;i<AbstractCommand::suspended_processes_.size();i++){
        output+=std::to_string(i)+" pid="+std::to_string(AbstractCommand::suspended_processes_[i])+'\n';
    }
}

void Command_Fg::run()
{
    auto args=preProcess();
    pid_t id=stoi(args[0]);
    kill(AbstractCommand::suspended_processes_[id],SIGQUIT);//给子进程发送信号
}

void Command_Shift::run()
{
    int num_shift=1;///移位的数字
    auto args=preProcess();
    if(!args.empty()){
        num_shift=stoi(args[0]);
    }
    auto shell_ptr=MyShell::GetSingletonPtr();
    for(int i=num_shift;i<10;i++){
        if(shell_ptr->env.find(std::to_string(i))!=shell_ptr->env.end()){//遍历并移位
            shell_ptr->env[std::to_string(i-num_shift)]=shell_ptr->env[std::to_string(i)];

        }
        else{
            shell_ptr->env.erase(std::to_string(i-1));
            break;
        }
    }
}

void Command_Environ::run()
{
    for(auto i:MyShell::GetSingletonPtr()->env){
        output+=i.first+"="+i.second+"\n";
    }
}
void Command_Help::run(){
    system(("more "+MyShell::GetSingletonPtr()->env["HELP_FILE"]).c_str());
}
