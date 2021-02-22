#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpro-net/gpro-net.h"

#include "Raknet/MessageIdentifiers.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/SocketIncludes.h"
#include "RakNet/RakNetSocket.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/GetTime.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace RakNet;
using namespace std;

class Room {

private:
	int roomID;
	std::unordered_map<std::string, std::string> ipUsernamesInRoom;
	int currentUsers = 0;
	int maxUsers = 10;

public:
	Room(int id);
	int getCurrentUsers() { return currentUsers; }
	void addUser(string ip, string username, RakNet::RakPeerInterface* peer, RakNet::Packet* packet);
	void sendMessageToRoom(string message, string senderIp, string senderUsername, RakNet::RakPeerInterface* peer, RakNet::Packet* packet);
	void sendUserList(string ip, RakNet::RakPeerInterface* peer, RakNet::Packet* packet);
};