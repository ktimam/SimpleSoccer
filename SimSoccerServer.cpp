// SimSoccerServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef WINDOWS

#include <stdio.h>
#include <iostream>

#include "constants.h"
#include "misc/utils.h"
#include "Time/PrecisionTimer.h"
#include "Game/EntityManager.h"
#include "SoccerPitch.h"
#include "SoccerTeam.h"
#include "Goalkeeper.h"
#include "FieldPlayer.h"
#include "FieldGoal.h"
#include "SteeringBehaviors.h"
#include "json/json.hpp"
#include "ParamLoader.h"
#include "Resource.h"

#include "misc/Snapshot.h"

#include "3rdparty/cpp-httplib/httplib.h"
#include "3rdparty/picojson/picojson.h"

using json = nlohmann::json;

const int MATCH_DURATION = 10;
const int MATCH_RATE = 6;

const int MILLI_IN_SECOND = 20;
const int MILLI_IN_MINUTE = 60 * 20;
const int SECOND_MAX_VALUE = 60;

const bool LOG_MATCH_OUTPUT = true;
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
std::string handle_advance(httplib::Client& cli, picojson::value data) {
    std::cout << "Received advance request data " << data << std::endl;
    std::cout << "Adding notice" << std::endl;
    auto payload = data.get("payload").get<std::string>();
    auto payloadstr = hex_to_string(payload.substr(2, payload.length() - 1));
    std::cout << "payloadstr : " << payloadstr << std::endl;

    auto seed = (unsigned)time(0);
    std::cout << "Seed Generated : " << seed << std::endl;
    //seed random number generator
    srand(1674374940);// seed);

    std::cout << "Creating Pitch..." << std::endl;

    g_SoccerPitch = new SoccerPitch(WindowWidth, WindowHeight);

    if (LOG_MATCH_OUTPUT)
    {
        g_MatchReplay = new Snapshot();
    }

    std::cout << "Starting Match..." << std::endl;
    mMatchFinished = false;
    int updates_count = 0;
    while (!mMatchFinished)
    {
        IncrementTime(1);
        //update game states
        g_SoccerPitch->Update();

        if (LOG_MATCH_OUTPUT)
        {
            updates_count++;
            //Don't take snapshot for every move
            if (updates_count % SNAPSHOT_RATE == 1 || updates_count == 1)
            {
                g_LastSnapshot = g_MatchReplay->AddSnapshot(g_SoccerPitch);
            }
        }
    }

    if (LOG_MATCH_OUTPUT)
    {
        json raw_data = g_MatchReplay->Snapshots();
        std::ofstream o("match_server.json");
        o << std::setw(4) << raw_data << std::endl;
    }

    auto score1 = g_SoccerPitch->HomeTeam()->OpponentsGoal()->NumGoalsScored();
    auto score2 = g_SoccerPitch->AwayTeam()->OpponentsGoal()->NumGoalsScored();

    if (LOG_MATCH_OUTPUT)
    {
        delete g_MatchReplay;
    }

    delete g_SoccerPitch;

    json result;
    result["seed"]   = seed;
    result["score1"] = score1;
    result["score2"] = score2;

    std::cout << "score1 : " << score1 << std::endl;
    std::cout << "score2 : " << score2 << std::endl;

    auto resulthex = to_hex_dump(result.dump());
    auto resultstr = std::string("\"") + std::string(resulthex) + std::string("\"");
    std::cout << "result : " << result.dump() << std::endl;
    std::cout << "resulthex : " << resulthex << std::endl;

    json notice;
    notice["payload"] = resulthex;
    std::cout << "notice : " << notice.dump() << std::endl;
    auto r = cli.Post("/notice", notice.dump(), "application/json");
    std::cout << "Received notice status " << r.value().status << " body " << r.value().body << std::endl;
    return "accept";
}

std::string handle_inspect(httplib::Client& cli, picojson::value data) {
    std::cout << "Received inspect request data " << data << std::endl;
    std::cout << "Adding report" << std::endl;
    auto payload = data.get("payload").get<std::string>();
    auto report = std::string("{\"payload\":\"") + payload + std::string("\"}");
    auto r = cli.Post("/report", report, "application/json");
    std::cout << "Received report status " << r.value().status << " body " << r.value().body << std::endl;
    return "accept";
}

int main(int argc, char** argv) {

    std::cout << "Server Started..." << std::endl;

    std::map<std::string, decltype(&handle_advance)> handlers = {
        {std::string("advance_state"), &handle_advance},
        {std::string("inspect_state"), &handle_inspect},
    };
    httplib::Client cli(getenv("ROLLUP_HTTP_SERVER_URL"));
    cli.set_read_timeout(20, 0);
    std::string status("accept");
    std::string rollup_address;
    while (true) {
        //std::cout << "Sending finish" << std::endl;
        auto finish = std::string("{\"status\":\"") + status + std::string("\"}");
        auto r = cli.Post("/finish", finish, "application/json");
        //std::cout << "Received finish status " << r.value().status << std::endl;
        if (r.value().status == 202) {
            //std::cout << "No pending rollup request, trying again" << std::endl;
        }
        else {
            picojson::value rollup_request;
            picojson::parse(rollup_request, r.value().body);
            picojson::value metadata = rollup_request.get("data").get("metadata");
            if (!metadata.is<picojson::null>() && metadata.get("epoch_index").get<double>() == 0 && metadata.get("input_index").get<double>() == 0) {
                rollup_address = metadata.get("msg_sender").get<std::string>();
                std::cout << "Captured rollup address: " << rollup_address << std::endl;
            }
            else {
                auto request_type = rollup_request.get("request_type").get<std::string>();
                auto handler = handlers.find(request_type)->second;
                auto data = rollup_request.get("data");
                status = (*handler)(cli, data);
            }
        }
    }
    return 0;
}

#endif