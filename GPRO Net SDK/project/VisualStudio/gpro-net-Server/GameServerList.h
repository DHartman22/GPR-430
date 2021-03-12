#pragma once

using namespace std;
#include <string>

struct GameServer {
	string serverIp;
	string serverPort;

	string serverName;
	string serverRegion;
	int currentPlayerCount;
	int maxPlayerCount;
};

class GameServerList {
private:
	GameServer gameServers[3];
public:
	void CheckServers(); //

};