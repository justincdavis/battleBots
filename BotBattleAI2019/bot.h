#ifndef BOT_H
#define BOT_H

#include <vector>
#include <memory>
#include "botcmd.h"
#include "botai.h"

class Bot
{
private:

    BotEvent     event{BotEventType::PowerUp, 0, 5, 5};
    std::string  receivedData;
    std::string  outgoingCommand;

public:
    std::unique_ptr<BotAI> brain;

public:

    Bot(std::unique_ptr<BotAI> brain);

    virtual ~Bot();

    void handleReceivedData(const std::string& str);
    std::string getOutgoingData() { std::string tmp = std::move(outgoingCommand); outgoingCommand.clear(); return tmp; }
    bool hasOutgoingData() { return outgoingCommand.size(); }

    std::string name() { return brain->name; }

protected:

    BotCmd handleEvents(BotEvent& event);

    BotCmd Turn(double angle);
    BotCmd Move(double speed, double time = 1000);
    BotCmd Fire();
    BotCmd Scan(double fieldOfView);
};

#endif // BOT_H
