#include <iostream>
#include <sstream>
#include "bot.h"

using namespace std;

Bot::Bot(std::unique_ptr<BotAI> brain)
    : brain{std::move(brain)}
{
}

Bot::~Bot()
{
}

BotCmd Bot::handleEvents(BotEvent& evt)
{
    return brain->handleEvents(evt);
}

void printEvent(std::string name, BotEvent& event)
{
    stringstream ss;
    event.write(ss);
    cout << "BOT: " << name << ": " << ss.str().c_str() << endl;
}

void Bot::handleReceivedData(const std::string& incoming)
{
    receivedData.append(incoming);

    auto idx = receivedData.find_first_of('\n');

    if (idx != string::npos) {
        string commandString{receivedData.substr(0, idx)};
        receivedData = receivedData.substr(idx+1);
        //cout << "Received: '" << commandString << endl;

        std::stringstream evtStream(commandString);
        event = BotEvent::read(evtStream);

        //printEvent(name(), event);

        BotCmd nextCommand = handleEvents(event);

        std::stringstream cmdStream;
        nextCommand.write(cmdStream);
        outgoingCommand = cmdStream.str();
        //cout << "Outgoing: ";
        //cout << outgoingCommand << endl;
    }
}
