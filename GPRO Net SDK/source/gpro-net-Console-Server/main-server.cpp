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

#include "gpro-net/gpro-net.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define MAX_CLIENTS 10
#define SERVER_PORT 7777

//IP, Username
std::unordered_map<std::string, std::string> ipUsernames;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

enum UsernameMessage
{
	ID_NEW_USER_JOINED = ID_GAME_MESSAGE_1 + 1
};

enum ServerMessage //A message sent from the server to the client privately, such as a user list
{
	ID_SERVER_MESSAGE = ID_NEW_USER_JOINED + 1
};

enum PrivateMessage //a message sent from another user only for a specific client
{
	ID_PRIVATE_MESSAGE = ID_SERVER_MESSAGE + 1
};

enum DisconnectMessage //sends when client is shut down
{
	ID_DISCONNECT_EVENT = ID_PRIVATE_MESSAGE + 1
};

enum DeclineJoinEvent //Turns away users who already have an identical ip connected
{
	ID_DECLINE_JOIN = ID_DISCONNECT_EVENT + 1
};

void logEvent(int timestamp, std::string message)
{
	std::ofstream file;
	file.open("uniquename.txt", std::ios::app);
	//file.open("log.txt");

	if (file.is_open())
	{
		//file << "Pain 2\n";
		file << "Time: " + std::to_string(timestamp) + " User: N/A Message: " + message << std::endl;
		//printf(GetCurrentDirectory());
	}

	
	file.close();
}

void disconnect(std::string ip)
{

	ipUsernames.erase(ip); //untested
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

// https://www.cplusplus.com/reference/unordered_map/unordered_map/find/
std::string validateUsername(std::string username)
{
	std::unordered_map<std::string, std::string>::const_iterator result = ipUsernames.find(username);

	if (result == ipUsernames.end())
	{
		return result->first;
	}
	else
		return "N/A";
}

int main(int const argc, char const* const argv[])
{
	//char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	
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
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");

				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOut.Write("Hello world");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			}
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
					printf("A client has disconnected.\n");

					disconnect(packet->systemAddress.ToString(false));

				break;
			case ID_CONNECTION_LOST:
					printf("A client lost connection.\n");

					disconnect(packet->systemAddress.ToString(false));

				break;
			case ID_GAME_MESSAGE_1:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				//const char * a = packet->systemAddress.ToString(false);	
				
				//printf("Message from " + packet->systemAddress);

				std::string ip = packet->systemAddress.ToString(false);

				//printf("%" PRINTF_64_BIT_MODIFIER "u ", packet->systemAddress);

				RakNet::Time time;

				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(time);
				//time /= 1000;

				std::string broadcastMessage = "";
				broadcastMessage += std::to_string((int)time);
				broadcastMessage += +" | [" + ipUsernames.find(ip)->second + "]: ";
				broadcastMessage += rs.C_String();

				std::cout << time;
				//printf("%" PRINTF_64_BIT_MODIFIER "u ", time);
				printf(" | ");

				std::cout << "[" << ipUsernames.find(ip)->second << "]: ";
				printf("%s\n", rs.C_String());

				logEvent((int)time, rs.C_String());

				//returnUsers(peer, packet);

				RakNet::BitStream bsBroadcast;
				bsBroadcast.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsBroadcast.Write(broadcastMessage);
				peer->Send(&bsBroadcast, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

			}
			break;
			case ID_TIMESTAMP:
			{
				RakNet::Time time;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				bsIn.Read(time);
				
				//bsIn.Read()
				//RakNet::RakString rs = time;

				//bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				//bsIn.Read(rs);
				//printf("Message from " + )
				int a = (int)time;
				time /= 1000;
				printf("%" PRINTF_64_BIT_MODIFIER "u ", time);
				printf("\n");
				//printf();
				//printf("%d\n", a);
			}
			break;
			case ID_NEW_USER_JOINED:
			{
				std::string username;
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs); 

				std::string ip = packet->systemAddress.ToString(false);


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

				std::string target = validateUsername(rsTarget.C_String());
				std::string targetIp;
				if (target != "N/A")
				{
					std::unordered_map<std::string, std::string>::iterator it = ipUsernames.begin();
					while (it != ipUsernames.end())
					{
						if (it->second == target)
						{
							targetIp = it->first;
							break;
						}
						it++;
					}
					RakNet::BitStream bsOutMessage;
					//RakNet::BitStream bsOutTimestamp;
					RakNet::Time timestamp = RakNet::GetTime();


					//sc.EncodeString
					bsOutMessage.Write((RakNet::MessageID)ID_PRIVATE_MESSAGE);
					bsOutMessage.Write(rsTarget);
					bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOutMessage.Write(rsMessage);
					bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
					bsOutMessage.Write(timestamp);

					RakNet::SystemAddress address = SystemAddress(targetIp.c_str());

					peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
				}
			}
			break;
			case ID_SERVER_MESSAGE:
			{
				returnUsers(peer, packet);
			}
			break;
			case ID_DISCONNECT_EVENT:
			{
				std::string ip = packet->systemAddress.ToString(false);
				disconnect(ip);
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
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
