#pragma once

using namespace std;
using namespace RakNet;
#include <string>
#include "gpro-net/gpro-net-server/gpro-net-RakNet-Server.hpp"


struct GameServer {
	string serverIp;
	string serverPort;

	string serverName;
	string serverRegion;
	bool currentlyActive;
	int currentPlayerCount;
	int maxPlayerCount;
};

class GameServerList {
private:
	GameServer gameServers[3];
public:
	void CheckServers(); //
	void SendServerListToClient(RakNet::SystemAddress client);
	GameServerList(GameServer servers[3]);
};