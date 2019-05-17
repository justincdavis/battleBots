#ifndef BOTAI_H
#define BOTAI_H

#include "botcmd.h"
#include <vector>

//enum class botMode {
//    searching,
//    evading,
//    attacking
//};

class BotAI
{
public:
    std::string name;
    static int botCount;
    int botNum;
public:
    BotAI();
    virtual ~BotAI();
    virtual BotCmd handleEvents(BotEvent& event) = 0;
    virtual std::string getName();
protected:
    void setName(const std::string& name);
    BotCmd Turn(double angle);
    BotCmd MoveForward(double timeInSeconds);
    BotCmd MoveBackward(double timeInSeconds);
    BotCmd Fire();
    BotCmd Scan(double fieldOfView);
    BotCmd Ignore();
};

#endif // BOTAI_H
