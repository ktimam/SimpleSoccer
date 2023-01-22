#pragma once

#include "SoccerPitch.h"
#include "SoccerTeam.h"
#include "PlayerBase.h"
#include "SoccerBall.h"
#include "json/json.hpp"

using json = nlohmann::json;

//#define POSITION_ENTRY std::unordered_map<std::string, std::vector<int>> 
//#define OUTFIT_ENTRY   std::unordered_map<std::string, std::string> 
//#define PLAYER_ENTRY   std::vector<std::unordered_map<int,int>

class Snapshot
{
private:
	//pointers to the match snapshots
	//std::vector<json>  m_Snapshots;
	json m_Snapshots;


public:
	const json Snapshots()const { return m_Snapshots; }

	json AddSnapshot(SoccerPitch* soccer_pitch);
};

