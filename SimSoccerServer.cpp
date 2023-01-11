// SimSoccerServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "constants.h"
#include "misc/utils.h"
#include "Time/PrecisionTimer.h"
#include "Game/EntityManager.h"
#include "SoccerPitch.h"
#include "SoccerTeam.h"
#include "Goalkeeper.h"
#include "FieldPlayer.h"
#include "SteeringBehaviors.h"
#include "Snapshot.h"
#include "json/json.hpp"
#include "misc/Cgdi.h"
#include "ParamLoader.h"
#include "Resource.h"
#include "misc/WindowUtils.h"
#include "debug/DebugConsole.h"

using json = nlohmann::json;

const int MATCH_DURATION = 90;
const int MATCH_RATE = 6;

const int MILLI_IN_SECOND = 20;
const int MILLI_IN_MINUTE = 60 * 20;
const int SECOND_MAX_VALUE = 60;

const int SNAPSHOT_RATE = 5;

int mTickCount = 0;
bool mMatchFinished = false;

SoccerPitch* g_SoccerPitch;
Snapshot* g_MatchReplay;
json         g_LastSnapshot;

void IncrementTime(int rate)
{
    mTickCount += MATCH_RATE * rate;

    int minutes = mTickCount / MILLI_IN_MINUTE;

    if (minutes >= MATCH_DURATION)
    {
        mMatchFinished = true;
    }
}
std::string GetCurrentTimeString()
{
    int seconds = (mTickCount / MILLI_IN_SECOND) % SECOND_MAX_VALUE;
    int minutes = mTickCount / MILLI_IN_MINUTE;
    std::ostringstream stringStream;
    stringStream << minutes << " : " << seconds;
    std::string time = stringStream.str();
    return time;
}

int main()
{
    std::cout << "Entered Main!" << std::endl;

    //seed random number generator
    srand((unsigned)time(0));

    std::cout << "Creating Pitch" << std::endl;

    g_SoccerPitch = new SoccerPitch(684, 341);

    std::cout << "Creating Snapshot" << std::endl;
    g_MatchReplay = new Snapshot();

    int updates_count = 0;

    std::cout << "Starting Match..." << std::endl;
    while (!mMatchFinished)
    {
        IncrementTime(1);

        //update game states
        g_SoccerPitch->Update();
        updates_count++;
        //Don't take snapshot for every move
        if (updates_count % SNAPSHOT_RATE == 1 || updates_count == 1)
        {
            g_LastSnapshot = g_MatchReplay->AddSnapshot(g_SoccerPitch);
        }
    }//end while
    std::cout << "Match Finished! Writing to file." << std::endl;
    // write prettified JSON to another file// , std::ios::out | std::ios::binary | std::ios::ate);
    json raw_data = g_MatchReplay->Snapshots();
    // create a JSON value
    //std::vector<std::uint8_t> v_bson = json::to_msgpack(raw_data);
    //std::ofstream obson("match.msgpack.json", std::ios::out | std::ios::binary | std::ios::ate);
    //// print the vector content
    //for (auto& byte : v_bson)
    //{
    //    obson << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    //}
    //obson << std::endl;

    std::ofstream o("matchlog.json");
    o << std::setw(4) << raw_data << std::endl;

    delete g_SoccerPitch;
}