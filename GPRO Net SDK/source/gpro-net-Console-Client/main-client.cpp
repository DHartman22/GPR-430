/*
Project 1: Server/Client Chat Application

Authors: Daniel Hartman and Joseph Castaner
*/

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

#include "gpro-net/gpro-net.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Raknet/MessageIdentifiers.h"
#include "RakNet/RakPeerInterface.h"
#include "RakNet/RakNetSocket.h"
#include "RakNet/SocketIncludes.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/GetTime.h"
#include "RakNet/StringCompressor.h"
#include <iostream>
#include <fstream>
#include <string>

#define MAX_CLIENTS 10
#define SERVER_PORT 7777
#define SERVER_IP "172.16.2.57"

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

enum UsernameMessage
{
	ID_NEW_USER_JOINED = ID_GAME_MESSAGE_1 + 1
};

enum ServerMessage //A message sent from the server to the client privately
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

//Modified version of the input menu which asks for a message and a singular target for said message
void inputPrivate(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	std::cout << "Type a message... or send a [/] without brackets to go back.\n";
	char dummy[100]; //dummy line to prevent cin skipping the first getline
	std::cin.getline(dummy, 100);
	char input[500], inputName[100];

	std::cin.getline(input, 500);
	if (input[0] != '/')
	{
		std::cout << "Type a username to send this message to.\n";
		std::cin.getline(inputName, 100); //get the name of the target
		RakNet::RakString rsUsername = inputName;
		
		RakNet::BitStream bsOutMessage;

		RakNet::Time timestamp = RakNet::GetTime();



		bsOutMessage.Write((RakNet::MessageID)ID_PRIVATE_MESSAGE); //send the required information to the server
		bsOutMessage.Write(rsUsername);
		bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOutMessage.Write(input);
		bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
		bsOutMessage.Write(timestamp);

		peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, SERVER_PORT), false); //4024?
	}
	else
	{
		return; //go back
	}
}

//Prompts user for input to send a message to the server, but also provides a menu with the option to query for a list of acrive users, or send a private message
void input(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	std::cout << "Type a message... or send a [/] without brackets for more options.\n";
	char input[500];
	input[0] = '\0';

	std::cin.getline(input, 500);
	if (input[0] != '/')
	{
		RakNet::BitStream bsOutMessage;
		RakNet::Time timestamp = RakNet::GetTime();

		//send the message to the server along with a timestamp
		bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOutMessage.Write(input);
		bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
		bsOutMessage.Write(timestamp);
		if (input[0] != '\0')
		{
			peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, 7777), false);
		}
	}
	else //if "/" is input, display this menu
	{
		std::cout << "Options: 1) Ask for list of active users 2) Send a Private Message 3) Go Back \n";
		int input2;
		std::cin >> input2;
		RakNet::BitStream bsQueryUserList; 
		switch (input2)
		{
		case 1: //Make a query and send to the server for the list
			bsQueryUserList.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
			peer->Send(&bsQueryUserList, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, 7777), false);
			break;
		case 2:
			inputPrivate(peer, packet);  //start the input process for a private message
			break;
		case 3:
			//nothing, if possible return
			break;
		default:
			printf("Input was not valid... Try again."); //let the user know if entry is not one of the available options
			break;
		}
	}
}

int main(int const argc, char const* const argv[])
{
	int loopsNeeded = 5000000; //update every 5000000 milliseconds
	int currentLoopCycle = 0;

	int lastInputTimecode = 1000;
	int timecodeRequirement = 1000;

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance(); //get the instance
	bool isServer;
	bool canInput = false;
	
	RakNet::SocketDescriptor sd; //start the socket
	peer->Startup(1, &sd, 1);
	isServer = false;
	RakNet::Packet* packet;
	
	printf("Starting the client.\n");
	peer->Connect(SERVER_IP, SERVER_PORT, 0, 0);

	//Prepares a bitstream to send the username to the server
	std::cout << "Enter your username. \n";
	char username[500];
		std::cin.getline(username, 500);
		
		std::cout << "Connecting...\n";
		RakNet::RakString rsUsername = username;
	RakNet::BitStream bsOutUsername;
	bsOutUsername.Write((RakNet::MessageID)ID_NEW_USER_JOINED); //write the username to the bitstream
	bsOutUsername.Write(rsUsername);
	
	//When a packet is received, decipher it's contents and act upon it
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
			case ID_CONNECTION_REQUEST_ACCEPTED: //Once a connection is confirmed, send username to the server
			{
				printf("Our connection request has been accepted.\n");


				canInput = true;
				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOutTime;
				RakNet::BitStream bsOutMessage;

				//timestamp connection
				RakNet::Time timeStamp;
				timeStamp = RakNet::GetTime();
				
				bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOutMessage.Write("New user connected: " + rsUsername);
				
				bsOutTime.Write((RakNet::MessageID)ID_TIMESTAMP);
				bsOutTime.Write(timeStamp);

				peer->Send(&bsOutUsername, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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
			case ID_GAME_MESSAGE_1: //Displays messages received from the chat room
			{
					RakNet::RakString rs; //read and print
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
			}
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
					printf("Connection attempt failed.\n");
					return 0;
				break;
			case ID_SERVER_MESSAGE: //Parses broadcasts from the server, displaying them onscreen
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());
			}
				break;
			case ID_PRIVATE_MESSAGE: //Parses private messages received, displaying time and sender as well
			{
				RakNet::RakString rsUsername;
				RakNet::RakString rsMessage;
				RakNet::Time timestamp;

				//read PM data
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rsUsername);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rsMessage);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(timestamp);

				//Print said data to the screen
				std::string broadcastMessage = "";
				broadcastMessage += std::to_string((int)timestamp);
				broadcastMessage += +" | ["; 
				broadcastMessage += rsUsername.C_String();
				broadcastMessage +=	+"]: ";
				broadcastMessage += rsMessage.C_String();

				std::cout << broadcastMessage << std::endl;

				
			}
			break;
			case ID_DECLINE_JOIN:
			{
				std::cout << "This address is already connected to the server.\n";
				return 0;
			}
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			
		}

		RakNet::TimeMS time = RakNet::GetTimeMS(); //Returns time in milliseconds for timing information
		if ((int)time - lastInputTimecode > timecodeRequirement && canInput)
		{
			input(peer, packet);
			lastInputTimecode = (int)time;
		}
	}

	//Disconnect from the server
	RakNet::BitStream bsDisconnect;
	bsDisconnect.Write((RakNet::MessageID)ID_DISCONNECT_EVENT);
	peer->Send(&bsDisconnect, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

	//cleanup
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
	printf("test message");
	printf("\n\n");
	system("pause");
}
