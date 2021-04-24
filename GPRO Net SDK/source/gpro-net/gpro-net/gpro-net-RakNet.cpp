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

	gpro-net-RakNet.cpp
	Source for RakNet common management.
*/

#include "gpro-net/gpro-net/gpro-net-RakNet.hpp"
#include <string.h>

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

enum PlayerStateUpdateEvent
{
	ID_PLAYER_STATE_UPDATE = ID_DECLINE_JOIN + 1
};

namespace gproNet
{
	cRakNetManager::cRakNetManager()
		: peer(RakNet::RakPeerInterface::GetInstance())
	{
	}

	cRakNetManager::~cRakNetManager()
	{
		RakNet::RakPeerInterface::DestroyInstance(peer);
	}

	bool cRakNetManager::ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID)
	{
		// process messages that can be handled the same way for all types of peers
		switch (msgID)
		{
		case ID_NEW_INCOMING_CONNECTION:
		{
			printf("A connection is incoming.\n");
		}
		break;
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("A client has disconnected.\n");
		}

		break;
		case ID_CONNECTION_LOST:
		{
			printf("A client lost connection.\n");

		}
		break;
		case ID_NEW_USER_JOINED: //Receives a username from any new connections, adding them to the list of current users and broadcasting their entry
		{

		}
		break;
		case ID_PRIVATE_MESSAGE: //Parses a private message from a client and directs the message to the specified target
		{

		}
		break;
		case ID_SERVER_MESSAGE: //Broadcasts the user list upon receiving a query
		{

		}
		break;
		case ID_DISCONNECT_EVENT: //take the user who disconnected and announce it
		{


			break;
		}
		}
		return false;
	}

	RakNet::BitStream& cRakNetManager::WriteTimestamp(RakNet::BitStream& bitstream)
	{
		bitstream.Write((RakNet::MessageID)ID_TIMESTAMP);
		bitstream.Write((RakNet::Time)RakNet::GetTime());
		return bitstream;
	}

	RakNet::BitStream& cRakNetManager::ReadTimestamp(RakNet::BitStream& bitstream, RakNet::Time& dtSendToReceive_out, RakNet::MessageID& msgID_out)
	{
		RakNet::Time tSend = 0, tReceive = 0;
		if (msgID_out == ID_TIMESTAMP)
		{
			tReceive = RakNet::GetTime();
			bitstream.Read(tSend);
			bitstream.Read(msgID_out);
			dtSendToReceive_out = (tReceive - tSend);
		}
		return bitstream;
	}

	RakNet::BitStream& cRakNetManager::WriteTest(RakNet::BitStream& bitstream, char const message[])
	{
		WriteTimestamp(bitstream);
		bitstream.Write((RakNet::MessageID)ID_GPRO_MESSAGE_COMMON_BEGIN);
		bitstream.Write(message);
		return bitstream;
	}

	RakNet::BitStream& cRakNetManager::ReadTest(RakNet::BitStream& bitstream)
	{
		RakNet::RakString rs;
		bitstream.Read(rs);
		printf("%s\n", rs.C_String());
		return bitstream;
	}

	int cRakNetManager::MessageLoop()
	{
		int count = 0;
		RakNet::Packet* packet = 0;
		RakNet::MessageID msgID = 0;
		RakNet::Time dtSendToReceive = 0;

		while (packet = peer->Receive())
		{
			RakNet::BitStream bitstream(packet->data, packet->length, false);
			bitstream.Read(msgID);

			// process timestamp
			ReadTimestamp(bitstream, dtSendToReceive, msgID);

			// process content
			if (ProcessMessage(bitstream, packet->systemAddress, dtSendToReceive, msgID))
				++count;

			// done with packet
			peer->DeallocatePacket(packet);
		}

		// done
		return count;
	}
}