#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H
#include "AbstractCommand.h"
#include <memory>   //提供内存操作的函数

class CommandFactory
{
public:
    CommandFactory(); //工厂类，处理命令字符串等
    std::shared_ptr<AbstractCommand> getCommand(string input);   //shared_ptr智能指针，当引用计数为0时，指向的对象将自动删除
};
#endif
