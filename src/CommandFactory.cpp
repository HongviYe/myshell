//简单工厂类：创建对象的接口。根据传入的参数，动态决定应该创建哪一个产品类的实例。
#include "CommandFactory.h"
#include <sstream>
#include <memory>
#include <fstream>
#include <iostream>
CommandFactory::CommandFactory()
{

}
/**
 * @brief 获取对应的执行器
 * 
 * @param input 某个命令的完整形式
 * @return std::shared_ptr<AbstractCommand> 
 */
std::shared_ptr<AbstractCommand> CommandFactory::getCommand(std::string input)
{
    std::shared_ptr<AbstractCommand> ptr;
    std::stringstream ss(input);
    string first_command;
    ss>>first_command;


    /*这里最好是用宏*/
    if(first_command=="cd")
    {
         ptr=std::make_shared<Command_CD>();  //make_shared模板函数的功能是在动态内存中分配一个对象并初始化它，返回指向此对象的shared_ptr;
    }
    else if(first_command=="dir")
    {
        ptr=std::make_shared<Command_Dir>();
    }
    else if(first_command=="time")
    {
        ptr=std::make_shared<Command_Time>();
    }
    else if(first_command=="pwd")
    {
        ptr=std::make_shared<Command_Pwd>();
    }
    else if(first_command=="clr")
    {
        ptr=std::make_shared<Command_Clr>();
    }
    else if(first_command=="exit")
    {
        ptr=std::make_shared<Command_Exit>();
    }
    else if(first_command=="quit")
    {
        ptr=std::make_shared<Command_Quit>();
    }
    else if(first_command=="exec")
    {
        ptr=std::make_shared<Command_Exec>();
    }
    else if(first_command=="echo")
    {
        ptr=std::make_shared<Command_Echo>();
    }
    else if(first_command=="jobs")
    {
        ptr=std::make_shared<Command_Jobs>();
    }
    else if(first_command=="fg")
    {
        ptr=std::make_shared<Command_Fg>();
    }
    else if(first_command=="shift")
    {
        ptr=std::make_shared<Command_Shift>();
    }
    else if(first_command=="environ")
    {
        ptr=std::make_shared<Command_Environ>();
    }
    else if(first_command=="help")
    {
        ptr=std::make_shared<Command_Help>();
    }
    else if(std::ifstream(first_command)||std::ifstream("./"+first_command)){
        input="exec "+input;
        ptr=std::make_shared<Command_Exec>();
    }
    else
        ptr=std::make_shared<Command_System>();
    ptr->setInput(input);
    return ptr;
}


