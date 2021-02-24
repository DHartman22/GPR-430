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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpro-net/gpro-net.h"
#include "gpro-net/gpro-net-common/gpro-net-gamestate.h"
#include "gpro-net/gpro-net-common/gpro-net-console.h"
#include "gpro-net/gpro-net-common/gpro-net-room.h"
#include <future>


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

#define MAX_CLIENTS 10
#define SERVER_PORT 7777
#define SERVER_IP "172.16.2.60"


using namespace std;
using namespace RakNet;

enum boardLHS
{
	A = 1,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J
};

class Battleship
{
public:
	Battleship();
	~Battleship();

	void init();
	void setShips();

	void input();
	void update();
	void display();

	void checkState();
	void gameEnd(); //check current turn to see who won

	void setTurn();
private:
	gpro_battleship mBoard;
	bool mCurrentTurn;
	bool mGameOver;
};

class BaseMessageClass
{
public:
	virtual BitStream& operator >> (BitStream& in) = 0; //read
	virtual BitStream& operator << (BitStream& out) = 0; //write
protected:
	NetworkID mNetID;
};

void inputPrivate(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	std::cout << "Type a message... or send a [/] without brackets to go back.\n";
	char dummy[100];
	std::cin.getline(dummy, 100);
	char input[500], inputName[100];
	//RakNet::RakString input;
	std::cin.getline(input, 500);
	if (input[0] != '/')
	{
		std::cout << "Type a username to send this message to.\n";
		std::cin.getline(inputName, 100);
		RakNet::RakString rsUsername = inputName;
		
		//packet = peer->Receive();
		//peer->DeallocatePacket(packet);
		//packet = peer->Receive();
		//RakNet::StringCompressor sc;
		RakNet::BitStream bsOutMessage;
		//RakNet::BitStream bsOutTimestamp;
		RakNet::Time timestamp = RakNet::GetTime();


		//sc.EncodeString
		bsOutMessage.Write((RakNet::MessageID)ID_PRIVATE_MESSAGE);
		bsOutMessage.Write(rsUsername);
		bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOutMessage.Write(input);
		bsOutMessage.Write((RakNet::MessageID)ID_TIMESTAMP);
		bsOutMessage.Write(timestamp);

		peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, SERVER_PORT), false); //4024?
	}
	else
	{
		return;
	}
}

void disconnect(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{

}


void logEvent()
{
	std::ofstream file;
	file.open("uniquename.txt", std::ios::app);
	//file.open("log.txt");

	std::ofstream myfile;
	myfile.open("example.txt");
	myfile << "Writing this to a file.\n";
	myfile.close();

	if (file.is_open())
	{
		file << "Pain\n";
		//file << "Time: " + std::to_string(timestamp) + " User: N/A Message: " + message << std::endl;
		//printf(GetCurrentDirectory());
	}


	file.close();
}

void input(RakNet::RakPeerInterface* peer, RakNet::Packet* packet)
{
	std::cout << "Type a message... or send a [/] without brackets for more options.\n";
	
	char input[500];
	input[0] = '\0';
	//RakNet::RakString input;
	std::cin.getline(input, 500);
	if (input[0] != '/')
	{
		//std::cout << input[0];
		
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
		if (input[0] != '\0')
		{
			peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, SERVER_PORT), false);
		}
	}
	else
	{
		std::cout << "Options: 1) Ask for list of active users 2) Send a Private Message 3) Join Room 4) Go Back 5) Get active users in your room 6) Return to the main lobby \n";
		int input2;
		std::cin >> input2;
		RakNet::BitStream inputBitStream; //doesnt like this declaration in the switch

		switch (input2)
		{
		case 1:
			inputBitStream.Write((RakNet::MessageID)ID_SERVER_MESSAGE);
			peer->Send(&inputBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, SERVER_PORT), false);
			break;
		case 2:
			inputPrivate(peer, packet);
			break;
		case 3:

			int inputRoom;
			cout << "Choose a room ID to join: [1], [2], [3], [4], or [5]. Type [-1] to go back.\n";
			cin >> inputRoom;
			while(inputRoom > 5 || inputRoom < 0)
			{
				if (inputRoom == -1)
				{
					return;
				}
				cout << "\nRoom index out of range. Eligible rooms are 1-5. Type -1 to return.\n";
				cin >> inputRoom;
			}
			inputBitStream.Write((RakNet::MessageID)ID_ROOM_JOIN);
			inputBitStream.Write(inputRoom);
			peer->Send(&inputBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, 7777), false);
			cout << "Joining Room " + to_string(inputRoom) + "...\n";
			break;
		case 4:
			
			//nothing, if possible return
			break;
		case 5:
			inputBitStream.Write((RakNet::MessageID)ID_REQUEST_ROOM_USER_LIST);
			inputBitStream.Write(1);
			peer->Send(&inputBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, 7777), false);
			break;
		case 6:
			inputBitStream.Write((RakNet::MessageID)ID_LEAVE_ROOM_REQUEST);
			peer->Send(&inputBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::SystemAddress(SERVER_IP, 7777), false);

			break;
		default:
			printf("Input was not valid... Try again.");
			break;
		}
	}
}

int main(int const argc, char const* const argv[])
{
	logEvent();


	int lastInputTimecode = 1000;
	int timecodeRequirement = 1000;
	//char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	bool canInput = false;
	
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	isServer = false;
	RakNet::Packet* packet;


	// TODO - Add code body here
	
	
	
	printf("Starting the client.\n");
	peer->Connect(SERVER_IP, SERVER_PORT, 0, 0);
		
	std::cout << "Enter your username. \n";
	char username[500];
		std::cin.getline(username, 500);
		//std::cin >> username;
		std::cout << "Connecting...\n";
		RakNet::RakString rsUsername = username;
	RakNet::BitStream bsOutUsername;
	bsOutUsername.Write((RakNet::MessageID)ID_NEW_USER_JOINED);
	bsOutUsername.Write(rsUsername);
	
	while (1)
	{
		//https://stackoverflow.com/questions/26127073/user-input-without-pausing-code-c-console-application
		//this showed me how to use async/future
		std::future<void> fut = std::async(std::launch::async, input, peer, packet);

		while (fut.wait_for(0.25s) != std::future_status::ready) {
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


					canInput = true;
					// Use a BitStream to write a custom user message
					// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
					RakNet::BitStream bsOutTime;
					RakNet::BitStream bsOutMessage;

					//RakNet::MessageID useTimeStamp;
					RakNet::Time timeStamp;
					timeStamp = RakNet::GetTime();
					//unsigned char useTimeStamp;
					bsOutMessage.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOutMessage.Write("New user connected: " + rsUsername);

					bsOutTime.Write((RakNet::MessageID)ID_TIMESTAMP);
					bsOutTime.Write(timeStamp);

					peer->Send(&bsOutUsername, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					//peer->Send(&bsOutTime, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					//peer->Send(&bsOutMessage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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
					return 0;
					break;
				case ID_SERVER_MESSAGE:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
				}
				break;
				case ID_PRIVATE_MESSAGE:
				{
					RakNet::RakString rsUsername;
					RakNet::RakString rsMessage;
					RakNet::Time timestamp;

					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rsUsername);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rsMessage);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(timestamp);

					std::string broadcastMessage = "";
					broadcastMessage += std::to_string((int)timestamp);
					broadcastMessage += +" | [";
					broadcastMessage += rsUsername.C_String();
					broadcastMessage += +"]: ";
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
		}

		RakNet::TimeMS time = RakNet::GetTimeMS();
		if ((int)time - lastInputTimecode > timecodeRequirement && canInput)
		{
		//	input(peer, packet);
			//lastInputTimecode = (int)time;
		}


	}

	RakNet::BitStream bsDisconnect;
	bsDisconnect.Write((RakNet::MessageID)ID_DISCONNECT_EVENT);
	peer->Send(&bsDisconnect, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);


	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
	printf("test message");
	printf("\n\n");
	system("pause");
}

//BATTLESHIP CLASS DEFINITIONS BELOW

Battleship::Battleship()
{
	mCurrentTurn = false;
	mGameOver = false;

	init();
}

Battleship::~Battleship()
{
	//destructor
}

void Battleship::init()
{
	const int BOARD_SIZE = 10;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			mBoard[i][j] = gpro_battleship_open;
		}
	}
}

void Battleship::setShips()
{
	std::string response1;
	std::string response2;
	std::string response3;
	std::string response4;
	std::string response5;
	
	std::string delimiter = ",";
	std::string letter, number;
	int iletter, inumber;
	bool horizontal = false;

	//ask user where to put ships
	cout << "Where would you like to place your Patrol Boat? (2 spaces)" << endl <<
		"Please use a letter followed by a number, then either 'v' or 'h' for the direction separated by a comma. Example: D4,h" << endl;
	cin >> response1;
	//get coords
	response1.substr(0, std::string::find());
	if ()
		iletter = ;
	mBoard[iletter][inumber] = gpro_battleship_ship_p2;
	if (horizontal)
	{
		mBoard[iletter][inumber+1] = gpro_battleship_ship_p2;
	}
	else
	{
		mBoard[iletter+1][inumber] = gpro_battleship_ship_p2;
	}

	cout << "All ships have been placed!" << endl;
}

void Battleship::input()
{
	//get move from user
	if (mCurrentTurn)
	{

	}
	else
	{
		//get other player data?
		cout << "It is not currently your turn..." << endl;
	}

	setTurn();
}

void Battleship::update()
{
	while (!mGameOver)
	{
		input();
		checkState();
		display();
	}
}

void Battleship::display()
{
	//display board
	const int BOARD_SIZE = 10;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if(mBoard[i][j] == gpro_battleship_open)
				cout << "|  |";
			else if (mBoard[i][j] == gpro_battleship_hit)
				cout << "|o |";
			else if (mBoard[i][j] == gpro_battleship_miss)
				cout << "|x |";
			else if (mBoard[i][j] == gpro_battleship_ship_p2)
				cout << "|P |";
			else if (mBoard[i][j] == gpro_battleship_ship_s3)
				cout << "|S |";
			else if (mBoard[i][j] == gpro_battleship_ship_d3)
				cout << "|D |";
			else if (mBoard[i][j] == gpro_battleship_ship_b4)
				cout << "|B |";
			else if (mBoard[i][j] == gpro_battleship_ship_c5)
				cout << "|C |";
		}
		cout << endl;
	}
}

void Battleship::checkState()
{
	const int BOARD_SIZE = 10;
	const int WIN_CON = 17;
	int currentPoints = 0;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (mBoard[i][j] == gpro_battleship_hit)
				currentPoints++;
		}
	}

	if (currentPoints == WIN_CON)
		gameEnd();
}

void Battleship::gameEnd()
{
	mGameOver = true;

	if (mCurrentTurn)
	{
		//win
		cout << "You have Won!" << endl;
	}
	else
	{
		//loss
		cout << "You have Lost!" << endl;
	}
}

void Battleship::setTurn()
{
	mCurrentTurn = !mCurrentTurn;
}
