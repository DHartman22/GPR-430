#include "GameServerList.h"

void GameServerList::CheckServers()
{
	//goes through the list of server IPs and pings all of them, asks for player count, name 
	//for every server in gameServers, send a message ID that requests active status, server name,
	//region, current and max player counts


}

void GameServerList::SendServerListToClient(RakNet::SystemAddress client)
{
	//for every server that is currently active, bundle that information into a rakString
	//and send it to the client
}

GameServerList::GameServerList(GameServer servers[3])
{
	//attach provided server information to gameServers variable
}
