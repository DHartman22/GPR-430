/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-server.c/.cpp
	Main source for console server application.
*/
//using namespace RakNet;



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
#include "gpro-net/gpro-net-common/gpro-net-room.h"

using namespace RakNet;
#define MAX_CLIENTS 10
#define SERVER_PORT 7777

//IP, Username
std::unordered_map<std::string, std::string> ipUsernames;
std::unordered_map<std::string, int> ipRoomIndexes;

Room roomList [5] = { Room(0), Room(1), Room(2), Room(3), Room(4) };



struct roomStruct
{
	int roomIndex; //0 = main lobby
	std::unordered_map<std::string, std::string> ipUsernamesInRoom;
	int currentUsers = 0;
	int maxUsers = 10;
};


void logEventUserMessage(int timestamp, string message, string username)
{
	ofstream file;
	file.open("log.txt", std::ios::app);

	if (file.is_open())
	{
		file << "Time: " + std::to_string(timestamp) + " User: " + " Message: " + message << std::endl;
	}

	file.close();
}

void logEvent(int timestamp, string rawMessage)
{
	ofstream file;
	file.open("log.txt", std::ios::app);

	if (file.is_open())
	{
		file << "Time: " + std::to_string(timestamp) + "|  " + rawMessage << std::endl;
	}

	file.close();
}

bool findUserToRemoveFromRoom(string ip)
{
	for (int i = 0; i < 5; i++)
	{
		if (roomList[i].removeUserFromRoom(ip))
		{
			return true;
		}
	}
	return false;
}

int isUserInRoom(string ip)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < roomList[i].getCurrentUsers(); j++)
		{
			if (roomList[i].isUserInRoom(ip))
			{
				return roomList[i].getRoomID();
			}
		}
	}
	return -1;
}

void disconnect(string ip)
{
	ipUsernames.erase(ip); //removes user from active users list
	findUserToRemoveFromRoom(ip);
}

void returnUsers(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	RakNet::RakString rs;
	std::string output = "\nUsers currently connected:\n";
	

	std::unordered_map<std::string, std::string>::iterator it = ipUsernames.begin();
	while(it != ipUsernames.end())
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

void returnUsersInLobby(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	RakNet::RakString rs;
	std::string output = "\nUsers currently connected:\n";


	std::unordered_map<std::string, std::string>::iterator it = ipUsernames.begin();
	while (it != ipUsernames.end())
	{
		if (isUserInRoom(it->first) == -1)
		{
			output.append(it->second + "\n");
		}
		it++;
	}

	std::cout << output;
	rs = output.c_str();

	RakNet::BitStream bsOutUsers;
	bsOutUsers.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
	bsOutUsers.Write(rs);
	peer->Send(&bsOutUsers, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

}


int main(int const argc, char const* const argv[])
{
	//char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	
	//roomStruct[5];
	SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	isServer = true;
	RakNet::Packet* packet;

	// TODO - Add code body here

	

	printf("Starting the server.\n");
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("A connection is incoming.\n");
				logEvent((int)GetTimeMS(), "A connection is incoming.");
			}
				break;
			case ID_DISCONNECTION_NOTIFICATION:
					printf("A client has disconnected.\n");
					logEvent((int)GetTimeMS(), "A client has disconnected.");

					disconnect(packet->systemAddress.ToString(true));

				break;
			case ID_CONNECTION_LOST:
			
					printf("A client lost connection.\n");
					logEvent((int)GetTimeMS(), "A client lost connection.");
					disconnect(packet->systemAddress.ToString(true));

				break;
			case ID_GAME_MESSAGE_1:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				//const char * a = packet->systemAddress.ToString(false);	
				
				string ip = packet->systemAddress.ToString(true);
				
				RakNet::Time time;

				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(time);

				std::string broadcastMessage = "";
				broadcastMessage += std::to_string((int)time);
				broadcastMessage += +" | [" + ipUsernames.find(ip)->second + "]: ";
				broadcastMessage += rs.C_String();

				std::cout << time;
				printf(" | ");

				cout << "[Room " << to_string(isUserInRoom(packet->systemAddress.ToString(true))) << "] ";

				cout << "[" << ipUsernames.find(ip)->second << "]: ";
				printf("%s\n", rs.C_String());

				logEventUserMessage((int)time, rs.C_String(), ipUsernames.find(ip)->second);

				RakString output = broadcastMessage.c_str();

				RakNet::BitStream bsBroadcast;
				bsBroadcast.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
				bsBroadcast.Write(output);
				int roomIndexOfUser = isUserInRoom(ip);
				if (roomIndexOfUser != -1)
				{
					roomList[roomIndexOfUser].sendMessageToRoom(broadcastMessage, ip, peer, packet);
				}
				else
				{
					std::unordered_map<std::string, std::string>::iterator it = ipUsernames.begin();
					while (it != ipUsernames.end()) // prevents users in rooms from receiving main lobby messages
					{
						if (isUserInRoom(it->first) == -1) 
						{
							string currentIp = it->first;
							std::size_t position = currentIp.find("|");

							std::string port = currentIp.substr(position + 1);

							SystemAddress recipient = SystemAddress(currentIp.c_str(), stoi(port));
							peer->Send(&bsBroadcast, HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipient, false);
						}
						it++;
					}
				}

			}
			break;
			case ID_NEW_USER_JOINED:
			{
				std::string username;
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs); 

				std::string ip = packet->systemAddress.ToString(true);


				bool taken = false;
				std::unordered_map<std::string, std::string>::iterator it = ipUsernames.begin();
				while (it != ipUsernames.end())
				{
					if (it->first == ip)
					{
						taken = true;
					}
					it++;
				}
				if (!taken)
				{
					ipUsernames.emplace(ip, rs.C_String());
				}
					std::string joinMessage = ipUsernames.find(ip)->second;
					joinMessage += " has joined the server.\n";

					std::cout << joinMessage << std::endl;

					logEvent((int)GetTimeMS(), joinMessage);

					std::size_t position = ip.find("|");

					std::string port = ip.substr(position+1);
					
					RakString broadcast = joinMessage.c_str();
					BitStream bsOutBroadcast;
					bsOutBroadcast.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
					bsOutBroadcast.Write(broadcast);


					peer->Send(&bsOutBroadcast, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			break;
			case ID_PRIVATE_MESSAGE:
			{
				std::string username;
				RakNet::RakString rsTarget;
				RakNet::RakString rsMessage;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rsTarget); 
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rsMessage);

				std::string target = rsTarget.C_String();
				std::string targetIp;
				targetIp = "null";
					std::unordered_map<std::string, std::string>::iterator it = ipUsernames.begin();
					while (it != ipUsernames.end())
					{
						if (it->second == rsTarget.C_String())
						{
							targetIp = it->first;
							break;
						}
						it++;
					}


					std::string ip = packet->systemAddress.ToString(true);
					std::string recipString = "From: ";
					recipString.append(ipUsernames.at(ip).c_str());
					RakString recipient = recipString.c_str();

					RakNet::BitStream bsOutMessage;
					RakNet::Time timestamp = RakNet::GetTime();
					bsOutMessage.Write((RakNet::MessageID)ID_PRIVATE_MESSAGE);
					bsOutMessage.Write(recipient);
					bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOutMessage.Write(rsMessage);
					bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
					bsOutMessage.Write(timestamp);


					std::size_t position = targetIp.find("|");

					std::string port = targetIp.substr(position + 1);

					if (targetIp != "null")
					{
						std::string consoleOutput = "";

						consoleOutput.append("Message from ");
						consoleOutput.append(recipient.C_String());
						consoleOutput.append(" to " + target + ": ");
						consoleOutput.append(rsMessage.C_String());

						logEvent((int)GetTimeMS(), consoleOutput);

						std::cout << consoleOutput << std::endl;
						RakNet::SystemAddress address = SystemAddress(targetIp.c_str(), std::stoi(port));
						peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
					}
					else
					{
						std::cout << "Unable to find user to send message.\n";
					}
			}
			break;
			case ID_SERVER_MESSAGE:
			{
				std::string ip = packet->systemAddress.ToString(true);
				std::string consoleOutput = ipUsernames.at(ip).c_str();
				consoleOutput.append(" requested a user list.");
				std::cout << consoleOutput;
				logEvent((int)GetTimeMS(), consoleOutput);

				returnUsers(peer, packet);
			}
			break;
			case ID_DISCONNECT_EVENT:
			{
				std::string ip = packet->systemAddress.ToString(true);
				std::string consoleOutput = ipUsernames.at(ip).c_str();
				consoleOutput.append(" disconnected.");
				std::cout << consoleOutput;
				logEvent((int)GetTimeMS(), consoleOutput);
				disconnect(ip);
			}
			break;
			case ID_REQUEST_ROOM_USER_LIST:
			{
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				int room;
				bsIn.Read(room);
				roomList[room].sendUserList(packet->systemAddress.ToString(true), peer, packet);
			}
			break;
			case ID_ROOM_JOIN:
			{
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				int roomToJoin;
				bsIn.Read(roomToJoin);

				int currentRoom = isUserInRoom(packet->systemAddress.ToString(true));
				if (currentRoom != -1)
				{
					BitStream bsOut;
					string warningString = "WARNING: You're already in Room " + to_string(currentRoom) + ".\n If you want to join another room, leave this room first by entering [/] followed by [6]";
					RakString warningRakString = warningString.c_str();
					bsOut.Write((MessageID)ID_SERVER_MESSAGE);
					bsOut.Write(warningRakString);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}
				else
				{
					roomList[roomToJoin].addUser(packet->systemAddress.ToString(true), ipUsernames.at(packet->systemAddress.ToString(true)), peer, packet);

				}


			}
			break;
			case ID_LEAVE_ROOM_REQUEST: 
			{
				bool success = findUserToRemoveFromRoom(packet->systemAddress.ToString(true));

				if (success)
				{
					BitStream bsOut;
					string warningString = "Welcome back to the lobby!";
					RakString warningRakString = warningString.c_str();
					bsOut.Write((MessageID)ID_SERVER_MESSAGE);
					bsOut.Write(warningRakString);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}
				else
				{
					BitStream bsOut;
					string warningString = "You're already in the lobby.";
					RakString warningRakString = warningString.c_str();
					bsOut.Write((MessageID)ID_SERVER_MESSAGE);
					bsOut.Write(warningRakString);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}
				
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				logEvent((int)GetTimeMS(), "Message with identifier % i has arrived.");
				break;
			}
		}
	}

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
	printf("test message");
	printf("\n\n");
	system("pause");
}
