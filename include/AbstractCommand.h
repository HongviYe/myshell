#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <time.h>
#include <memory>
using std::string;
class MyShell;
/**
 * @brief 基础抽象命令类，一个对象对应一个命令
 * 
 */
class AbstractCommand{
public:
    static std::map<string,string> color_map;
    AbstractCommand();
    virtual void createEnvirment()=0; //纯虚函数，生成运行环境，若为外部命令则生成子进程完成
    virtual void run()=0;   //纯虚函数，表示此基类只能被继承，而不能被实例化。在此不做实现，仅作为一个接口，而在派生类中必须被定义
    void setInput(string input);
    string getOutput();
    /**
     * @brief setColorFlag,false--plain text, true--colorful text
     */
    void setColorFlag(bool use_color);

/**
 * @brief 获取返回值
 * 
 * @return int 
 */
    int getReturnState();

    static void processSuspend(int id);
    static void Abort(int id);
    static void nothing(int id);
protected:
    std::vector<string> preProcess();
    string getpwd();
    string addColor(string target,string color);
protected:


    static std::vector<pid_t > suspended_processes_;
    int return_state_bit_;///1--run failed 0--succeed
    bool add_color_;///default:true show color in text, false: show plain text
    string input_line;
    string output;
    string key;/// 关键词
    /**
     * @brief 子进程活父进程（fork产生的）PID
     * 
     */
    pid_t new_process_pid;
    static pid_t tmp;
    /**
     * @brief 静态函数，由某些signal触发
     * 
     * @param id 
     */
    static void subprocessSuspend(int id);
};
/**
 * @brief 内部命令类
 * 
 */
class InternalCommand:public AbstractCommand{
    virtual void createEnvirment() override;
};
/**
 * @brief 外部命令类
 * 
 */
class OuterCommand:public AbstractCommand{
public:
    OuterCommand();
    virtual void createEnvirment() override;
public:
    bool background_;
};
class Command_System:public OuterCommand{
public:
    virtual void run() override;

};

class Command_CD:public InternalCommand{
public:
    Command_CD() {key="cd";}
    virtual void run() override;
};

class Command_Dir:public InternalCommand{
public:
    Command_Dir(){key="dir";}
    virtual void run()  override;
};

class Command_Time:public InternalCommand{
public:
    Command_Time(){key="time";}
    virtual void run()  override;
};

class Command_Pwd:public InternalCommand{
public:
    Command_Pwd() {key="pwd";}
    virtual void run();
};

class Command_Clr:public InternalCommand{
public:
    Command_Clr() {key="clr";}
    virtual void run()  override;
};

class Command_Quit:public  InternalCommand{
public:
    Command_Quit() {key="quit";}
    virtual void run();
};

class Command_Exit:public  InternalCommand{
public:
    Command_Exit() {key="exit";}
    virtual void run();
};

class Command_Exec:public OuterCommand{
public:
    Command_Exec() {key="exec";}
     virtual void run();
};
class Command_Echo:public InternalCommand{
public:
    Command_Echo() {key="echo";}
     virtual void run();
};
class Command_Jobs:public InternalCommand{
public:
    Command_Jobs() {key="jobs";}
     virtual void run();
};
class Command_Fg:public InternalCommand{
public:
    Command_Fg() {key="fg";}
     virtual void run();
};
class Command_Shift:public InternalCommand{
public:
    Command_Shift() {key="shift";}
     virtual void run();
};
class Command_Environ:public InternalCommand{
public:
    Command_Environ() {key="environ";}
     virtual void run();
};
class Command_Help:public InternalCommand{
public:
    Command_Help() {key="help";}
     virtual void run();
};
#endif
