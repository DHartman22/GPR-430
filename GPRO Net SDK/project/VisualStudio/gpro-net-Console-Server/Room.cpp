#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "..\..\..\include\gpro-net\gpro-net-common\gpro-net-room.h"
#include "gpro-net/gpro-net-common/gpro-net-room.h"

Room::Room(int id)
{
	roomID = id;
	p1IP = "";
	p2IP = "";
}

bool Room::removeUserFromRoom(string ipToCompare)
{
	std::unordered_map<std::string, std::string>::iterator it = ipUsernamesInRoom.begin();
	while (it != ipUsernamesInRoom.end())
	{
		if (ipToCompare == it->first)
		{
			ipUsernamesInRoom.erase(ipToCompare);
			currentUsers--;
			if (p1IP == ipToCompare)
			{
				p1IP = "";
				//sendMessageToRoom("P1 dropped out, next player to join replaces them", mPeer); mPeer not properly implemented yet
			}
			else if (p2IP == ipToCompare)
			{
				p2IP = "";
				//sendMessageToRoom("P2 dropped out, next player to join replaces them", mPeer);
			}
			return true;
		}
		it++;
	}
	return false;
}

bool Room::isUserInRoom(string ipToCompare)
{
	std::unordered_map<std::string, std::string>::iterator it = ipUsernamesInRoom.begin();
	while (it != ipUsernamesInRoom.end())
	{
		if (ipToCompare == it->first)
		{
			return true;
		}
		it++;
	}
	return false;
}


void Room::addUser(string ip, string username, RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	ipUsernamesInRoom.emplace(ip, username);
	currentUsers++;

	BitStream bsOut;
	string warningString = "You have joined Room " + to_string(roomID) + ". Say hello!";
	RakString warningRakString = warningString.c_str();
	bsOut.Write((MessageID)ID_SERVER_MESSAGE);
	bsOut.Write(warningRakString);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	sendUserList(packet->systemAddress.ToString(true), peer, packet);
}

void Room::bindUserToPlayerIP(string ip)
{
	if (p1IP == "")
	{
		p1IP = ip;
	}
	else if (p2IP == "")
	{
		p2IP = ip;
	}
}

void Room::sendMessageToRoom(string message, string ip, RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	RakNet::RakString rs = message.c_str();
	std::string output = "\n Room " + to_string(roomID) + ":";

	//std::cout << output;
	rs = message.c_str();
	//cout << rs.C_String() << endl;
	std::unordered_map<std::string, std::string>::iterator it = ipUsernamesInRoom.begin();
	while (it != ipUsernamesInRoom.end())
	{
		string currentIp = it->first;
		std::size_t position = currentIp.find("|");
 
		std::string port = currentIp.substr(position + 1);

		//if it finds one of the current game players, skip them, seperate function will be used
		//for player vs player messaging

		SystemAddress recipient = SystemAddress(currentIp.c_str(), stoi(port));

		RakNet::BitStream bsOutUsers;
		bsOutUsers.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
		bsOutUsers.Write(rs);
		peer->Send(&bsOutUsers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipient, false);
		//output.append(it->second + "\n");
		it++;
	}

}

void Room::sendMessageToOpponent(string message, string senderIp, RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	if (senderIp == p1IP)
	{
		RakString output = message.c_str();
		std::size_t position = p2IP.find("|");

		std::string port = p2IP.substr(position + 1);

		SystemAddress recipient = SystemAddress(p2IP.c_str(), stoi(port));

		RakNet::BitStream bsOutUsers;
		bsOutUsers.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOutUsers.Write(output);
		peer->Send(&bsOutUsers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipient, false);
	}
	else if (senderIp == p2IP)
	{
		RakString output = message.c_str();
		std::size_t position = p1IP.find("|");

		std::string port = p1IP.substr(position + 1);

		SystemAddress recipient = SystemAddress(p1IP.c_str(), stoi(port));

		RakNet::BitStream bsOutUsers;
		bsOutUsers.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOutUsers.Write(output);
		peer->Send(&bsOutUsers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipient, false);
	}
}

void Room::sendServerMessageToUser(string message, string recipientIp, RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	RakString output = message.c_str();
	std::size_t position = p2IP.find("|");

	std::string port = p2IP.substr(position + 1);

	SystemAddress recipient = SystemAddress(recipientIp.c_str(), stoi(port));

	RakNet::BitStream bsOutUsers;
	bsOutUsers.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
	bsOutUsers.Write(output);
	peer->Send(&bsOutUsers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipient, false);
}

void Room::sendUserList(string ip, RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	RakNet::RakString rs;
	std::string output = "\nUsers in room:\n";


	std::unordered_map<std::string, std::string>::iterator it = ipUsernamesInRoom.begin();
	while (it != ipUsernamesInRoom.end())
	{
		output.append(it->second + "\n");
		it++;
	}



	std::cout << output;
	rs = output.c_str();

	RakNet::BitStream bsOutUsers;
	bsOutUsers.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
	bsOutUsers.Write(rs);
	peer->Send(&bsOutUsers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}
