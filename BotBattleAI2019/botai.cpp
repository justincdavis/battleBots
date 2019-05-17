#include "botai.h"
#include <iostream>
#include <cmath>

using namespace std;

int BotAI::botCount = 0;

BotAI::BotAI()
{
    botNum = ++botCount;
    setName("Unnamed");
}

BotAI::~BotAI()
{
}

void BotAI::setName(const std::string& botName)
{
    name = botName + to_string(botNum);
}

std::string BotAI::getName()
{
    return name;
}

BotCmd BotAI::Turn(double angle)
{
    return BotCmd{BotCmdType::Turn, angle, 0};
}

BotCmd BotAI::MoveForward(double time)
{
    return BotCmd{BotCmdType::Move, 50, time};
}

BotCmd BotAI::MoveBackward(double time)
{
    return BotCmd{BotCmdType::Move, -50, time};
}

BotCmd BotAI::Fire()
{
    return BotCmd{BotCmdType::Fire, 0, 0};
}

BotCmd BotAI::Scan(double fieldOfView)
{
    return BotCmd{BotCmdType::Scan, fieldOfView, 0};
}

BotCmd BotAI::Ignore()
{
    return BotCmd{BotCmdType::Ignore, 0, 0};
}

