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

	main-client.c/.cpp
	Main source for console client application.
*/

//using namespace RakNet;

#include "gpro-net/gpro-net.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//using namespace RakNet;

#include "Raknet/MessageIdentifiers.h"
#include "RakNet/RakPeerInterface.h"
#include "RakNet/RakNetSocket.h"
#include "RakNet/SocketIncludes.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/GetTime.h"
#include "RakNet/StringCompressor.h"
#include <iostream>
#include <string>

#define MAX_CLIENTS 10
#define SERVER_PORT 4024
#define SERVER_IP "172.16.2.65"

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

enum UsernameMessage
{
	ID_NEW_USER_JOINED = ID_GAME_MESSAGE_1 + 1
};

void input(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	std::cout << "Type a message... or send a [/] without brackets for more options.\n";
	char input[500];
	//RakNet::RakString input;
	std::cin.getline(input, 500);
	if (input[0] != '/')
	{
		//packet = peer->Receive();
		//peer->DeallocatePacket(packet);
		//packet = peer->Receive();
		//RakNet::StringCompressor sc;
		RakNet::BitStream bsOutMessage;
		//RakNet::BitStream bsOutTimestamp;
		RakNet::Time timestamp = RakNet::GetTime();


		//sc.EncodeString
		bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOutMessage.Write(input);
		bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
		bsOutMessage.Write(timestamp);

		peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, 4024), false);
	}
}

int main(int const argc, char const* const argv[])
{
	//char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;

	
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	isServer = false;
	RakNet::Packet* packet;


	// TODO - Add code body here
	
	
	
	printf("Starting the client.\n");
	peer->Connect(SERVER_IP, SERVER_PORT, 0, 0);

	std::cout << "Enter your username. \n";
		std::string username;
	std::cin >> username;

	RakNet::BitStream bsOutUsername;
	bsOutUsername.Write((RakNet::MessageID)ID_NEW_USER_JOINED);
	bsOutUsername.Write(username);

	
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
				RakNet::BitStream bsOutTime;
				RakNet::BitStream bsOutMessage;

				//RakNet::MessageID useTimeStamp;
				RakNet::Time timeStamp;
				timeStamp = RakNet::GetTime();
				//unsigned char useTimeStamp;
				bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOutMessage.Write("New user connected");
				
				bsOutTime.Write((RakNet::MessageID)ID_TIMESTAMP);
				bsOutTime.Write(timeStamp);
				
				peer->Send(&bsOutTime, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			}
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
					printf("We have been disconnected.\n");
				break;
			case ID_CONNECTION_LOST:
					printf("Connection lost.\n");
				break;
			case ID_GAME_MESSAGE_1:
			{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
			}
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
					printf("Connection attempt failed.\n");
				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			
		}

		input(peer, packet);
		////this pauses the program rather than allowing it to refresh automatically
		////so its a temporary solution, idk how else to do it for now
		//std::cout << "Press Enter to refresh, or type a message to send to the server.\n";
		//char input[500];
		////RakNet::RakString input;
		//std::cin.getline(input, 500);
		//if (input[0] != '/')
		//{
		//	packet = peer->Receive();
		//	peer->DeallocatePacket(packet);
		//	//packet = peer->Receive();
		//	//RakNet::StringCompressor sc;
		//	RakNet::BitStream bsOutMessage;
		//	RakNet::BitStream bsOutTimestamp;
		//	RakNet::Time timestamp = RakNet::GetTime();


		//	//sc.EncodeString
		//	bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
		//	bsOutMessage.Write(timestamp);
		//	bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		//	bsOutMessage.Write(input);

		//	peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		//}
	}



	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
	printf("test message");
	printf("\n\n");
	system("pause");
}
