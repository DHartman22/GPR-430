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

	gpro-net-RakNet-Client.hpp
	Header for RakNet client management.
*/

#ifndef _GPRO_NET_RAKNET_CLIENT_HPP_
#define _GPRO_NET_RAKNET_CLIENT_HPP_
#ifdef __cplusplus


#include "gpro-net/gpro-net/gpro-net-RakNet.hpp"
#include "gpro-net/gpro-net/PlayerState.h"
#include <string>

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
	// eMessageClient
	//	Enumeration of custom client message identifiers.
	enum eMessageClient
	{
		ID_GPRO_MESSAGE_CLIENT_BEGIN = ID_GPRO_MESSAGE_COMMON_END,



		ID_GPRO_MESSAGE_CLIENT_END
	};
	

	// cRakNetClient
	//	RakNet peer management for server.
	class cRakNetClient : public cRakNetManager
	{
		// public methods
	public:
		// cRakNetClient
		//	Default constructor.
		cRakNetClient();

		std::string GetUsername() { return username; };

		PlayerState state;
		bool isPlayer1;
		// ~cRakNetClient
		//	Destructor.
		virtual ~cRakNetClient();

		RakNet::SystemAddress serverAddress;
		
		bool setServerAddress = false;

		void sendPlayerState();
		// protected methods
	protected:
		// ProcessMessage
		//	Unpack and process packet message.
		//		param bitstream: packet data in bitstream
		//		param dtSendToReceive: locally-adjusted time difference from sender to receiver
		//		param msgID: message identifier
		//		return: was message processed
		virtual bool ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);

		bool sendUsername(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);
	

		std::string username;

	};

}


#endif	// __cplusplus
#endif	// !_GPRO_NET_RAKNET_CLIENT_HPP_