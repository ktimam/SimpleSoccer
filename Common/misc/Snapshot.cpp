#include "Snapshot.h"

//JSON Format
// {
//  "Entities": {
//    {"Model": "ball", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "green", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "black", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]}
//  },
//  "Entities": {
//    {"Model": "ball", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "green", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "black", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]}
//  }
// }

json Snapshot::AddSnapshot(SoccerPitch* soccer_pitch)
{
    json entities;
    json ball;

    ball["id"] = "0";
    ball["p"] = { soccer_pitch->Ball()->Pos().x, soccer_pitch->Ball()->Pos().y, 0 };
    ball["h"] = { soccer_pitch->Ball()->Heading().x, soccer_pitch->Ball()->Heading().y, 0 };
    entities.push_back(ball);

    std::vector<PlayerBase*>::const_iterator it = soccer_pitch->HomeTeam()->Members().begin();
    for (it; it != soccer_pitch->HomeTeam()->Members().end(); ++it)
    {
        json player;

        player["id"] = (*it)->ID();
        player["p"] = { (*it)->Pos().x, (*it)->Pos().y, 0 };
        player["h"] = { (*it)->Heading().x, (*it)->Heading().y, 0 };
        entities.push_back(player);
        
    }

    it = soccer_pitch->AwayTeam()->Members().begin();
    for (it; it != soccer_pitch->AwayTeam()->Members().end(); ++it)
    {
        json player;

        player["id"] = (*it)->ID();
        player["p"] = { (*it)->Pos().x, (*it)->Pos().y, 0 };
        player["h"] = { (*it)->Heading().x, (*it)->Heading().y, 0 };
        entities.push_back(player);

    }

    m_Snapshots.push_back(entities);
    return entities;
}
