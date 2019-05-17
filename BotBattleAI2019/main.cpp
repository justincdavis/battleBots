#include "botmanager.h"
#include "networkplugin.h"

using namespace mssm;
using namespace std;

#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wnarrowing"

/*
 *
 * Bot Event Types

 Not-Ignorable  (If you ignore these, you'll go to "sleep"
                 until someone hits you or shoots you... not good)

    BotEventType::TurnComplete:      // you just finished a turn
    BotEventType::MoveComplete:      // you just finished a move
    BotEventType::FireComplete:      // you just finished a fire
    BotEventType::NoBullets:         // you attempted to fire but had no bullets
    BotEventType::ScanComplete:      // you just finished a scan
    BotEventType::MoveBlockedByBot:  // you ran into a bot while moving
    BotEventType::MoveBlockedByWall: // you ran into a wall while moving

    For any of the Not-Ignorable events, you should return one of these commands

    Move
    Turn
    Scan
    Fire

  Ignorable events:
    These are "interrupt" points, where you can either issue a new command (canceling
    your last command, or you can Ignore to allow your previous command to finish

    BotEventType::HitByBot:           // a bot ran into you
    BotEventType::HitByBullet:        // you got hit by a bullet
    BotEventType::PowerUp:            // you got a power up (more bullets!)

    For any of the Ignorable events, you should return one of these commands

    Move
    Turn
    Scan
    Fire
    Ignore
*/

enum class BotMode {
    searching,
    attacking,
    evading
};

class MyBotAI : public BotAI
{
    BotMode mode{BotMode::searching};

public:
    MyBotAI();
    virtual BotCmd handleEvents(BotEvent& event);
    BotCmd searchMode(BotEvent& event);
    BotCmd attackMode(BotEvent& event);
    BotCmd evadeMode(BotEvent& event);
private:
    bool justHitWall = false;
    double turnToShoot(vector<int> scanData);
};

MyBotAI::MyBotAI()
{
    setName("BestBot");
}

//vector<vector<int>> MyBotAI::checkScanForBot(vector<int> scanData){
//    vector<int> leftEdges;
//    vector<int> widths;
//    bool countingBotWidth = false;
//    int currentBotColor = 0;
//    int currentMeasuredWidth = 0;
//    int botsCounted = 0;

//    for(int i = 0; i <= scanData.size(); i++){
//        if(scanData[i] != 0 && !countingBotWidth){
//            leftEdges.push_back(i);
//            countingBotWidth = true;
//            currentBotColor = scanData[i];
//            botsCounted++;
//        }
//        if(countingBotWidth && scanData[i] == currentBotColor){
//            if(currentMeasuredWidth == 0){
//                widths.push_back(0);
//            } else {
//                widths[botsCounted-1]++;
//            }
//        } else {
//            currentBotColor = 0;
//            currentMeasuredWidth = 0;
//            countingBotWidth = false;
//        }
//    }

//    vector<vector<int>> returnData;
//    for(int i = 0; i < leftEdges.size(); i++){
//        vector<int> botData = {leftEdges[i], widths[i]};
//        returnData.push_back(botData);
//    }

//    return returnData;
//}

//vector<vector<int>> MyBotAI::removeDataUnderThreshhold(vector<vector<int>> botData, int scanDataSize, double threshhold){
//    for(int i = 0; i < botData.size(); i++){
//        if(botData[i][1]/scanDataSize < threshhold){
//            botData.erase(botData.begin() + i);
//        }
//    }
//    return botData;
//}

//double MyBotAI::turnToShoot(vector<int> scanData, double threshhold){
//    vector<vector<int>> botData = checkScanForBot(scanData);
//    botData = removeDataUnderThreshhold(botData, scanData.size(), threshhold);

//    int biggestBotIndex = 0;

//    for(int i = 0; i < botData.size(); i++){
//        if(botData[i][1] > botData[biggestBotIndex][1]){
//            biggestBotIndex = i;
//        }
//    }

//    if(botData.size() == 0){
//        return 0;
//    }

//    int centerOfWidth = botData[biggestBotIndex][0] + botData[biggestBotIndex][1]/2;
//    int diffFromCenterOfScan = scanData.size()/2 - centerOfWidth;
//    double radiansToTurn = recentScanRadians * (diffFromCenterOfScan/scanData.size());
//    return radiansToTurn;
//}

double MyBotAI::turnToShoot(vector<int> scanData){
    vector<int> indexOfBots;
    for(int i = 0; i < scanData.size(); i++){
        if(scanData[i] != 0){
            indexOfBots.push_back(i);
        }
    }
    int sumOfIndexes = 0;
    for(int i = 0; i < indexOfBots.size(); i++){
        sumOfIndexes = sumOfIndexes + indexOfBots[i];
    }
    if(indexOfBots.size() == 0){
        return 0;
    }
    double averageIndex = sumOfIndexes/indexOfBots.size();
    double diffFromCenter = 250 - averageIndex;
    double radiansToTurn = (diffFromCenter / 500.0);
    return radiansToTurn;
}

BotCmd MyBotAI::searchMode(BotEvent& event)
{
    switch (event.eventType)
    {
    case BotEventType::TurnComplete:
        return Scan(1);
    case BotEventType::MoveComplete:
        if(justHitWall){
            justHitWall = false;
            return Turn(1.5);
        }
        return Scan(1);
    case BotEventType::FireComplete:
        return Scan(1);
    case BotEventType::NoBullets:
    case BotEventType::ScanComplete:
        if(turnToShoot(event.scanData) != 0){
            mode = BotMode::attacking;
            return Turn(turnToShoot(event.scanData));
        } else {
            return MoveForward(1.5);
        }
    case BotEventType::MoveBlockedByBot:
        mode = BotMode::attacking;
        return Fire();
    case BotEventType::MoveBlockedByWall:
        justHitWall = true;
        return MoveBackward(1.5);
    case BotEventType::HitByBot:
        mode = BotMode::attacking;
        return MoveBackward(1.5);
    case BotEventType::HitByBullet:
        return MoveBackward(1.5);
    case BotEventType::PowerUp:
        return Ignore();
    }

    cout << "Oops!  You forgot to return a command!" << endl;

    return Ignore();
}

BotCmd MyBotAI::attackMode(BotEvent& event)
{
    switch (event.eventType)
    {
    case BotEventType::MoveComplete:
        if(justHitWall){
            justHitWall = false;
            return Turn(1);
        }
        return Scan(1);
    case BotEventType::TurnComplete:
        return Fire();
    case BotEventType::FireComplete:
        mode  = BotMode::searching;
        return MoveBackward(1);
    case BotEventType::NoBullets:
        mode = BotMode::searching;
        return MoveForward(1);
    case BotEventType::ScanComplete:
        if(turnToShoot(event.scanData) != 0){
            return Turn(turnToShoot(event.scanData));
        } else {
           return MoveForward(.5);
        }
    case BotEventType::MoveBlockedByBot:
        return Fire();
    case BotEventType::MoveBlockedByWall:
        justHitWall = true;
        return MoveBackward(1.5);
    case BotEventType::HitByBot:
        mode = BotMode::searching;
        return MoveBackward(2);
    case BotEventType::HitByBullet:
        return MoveBackward(1.5);
    case BotEventType::PowerUp:
        return Scan(1);
    }

    cout << "Oops!  You forgot to return a command!" << endl;

    return Ignore();
}

BotCmd MyBotAI::evadeMode(BotEvent& event)
{
    switch (event.eventType)
    {
    case BotEventType::MoveComplete:
    case BotEventType::TurnComplete:
    case BotEventType::FireComplete:
    case BotEventType::NoBullets:
    case BotEventType::ScanComplete:
    case BotEventType::MoveBlockedByBot:
    case BotEventType::MoveBlockedByWall:
    case BotEventType::HitByBot:
    case BotEventType::HitByBullet:
    case BotEventType::PowerUp:
        return Ignore();
    }

    cout << "Oops!  You forgot to return a command!" << endl;

    return Ignore();
}

BotCmd MyBotAI::handleEvents(BotEvent& event)
{
    switch (mode) {
    case BotMode::attacking:
        return attackMode(event);
    case BotMode::searching:
        return searchMode(event);
    case BotMode::evading:
        return evadeMode(event);
    default:
        cout << "Uh oh..." << endl;
        return Ignore();
    }
}

void graphicsMain(Graphics& g)
{
    BotManager botManager(g, 1233, "192.168.6.197" /*"localhost"*/);

    botManager.addBot(std::make_unique<MyBotAI>());

    while (g.draw())
    {
        g.clear();

        for (const Event& e : g.events())
        {
            cout << e << endl;

            if (botManager.processEvent(e)) {
                continue;
            }

            switch (e.evtType)
            {
            case EvtType::PluginMessage:
                if (e.arg == 1) {  // Probably a network change event
                    if (e.x == 3) { // network error (couldn't connect to server?) }
                        cout << "Network Error: " << e.data << endl;
                    }
                    else {
                        cout << "Network State: " << e.data << endl;
                    }
                }
                break;
            case EvtType::KeyPress:
                break;
            default:
                break;
            }
        }


    }
}

int main()
{
    Graphics g("Bot Battle Client", 800, 400, graphicsMain);
}
