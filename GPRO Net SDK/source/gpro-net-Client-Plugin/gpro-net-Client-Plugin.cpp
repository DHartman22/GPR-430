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

	gpro-net-Client-Plugin.cpp
	Main implementation for plugin; instantiate and manage networking.
*/

#include "gpro-net-Client-Plugin.h"
#include "gpro-net/gpro-net-client/gpro-net-RakNet-Client.hpp"


GPRO_NET_SYMBOL gproNet::cRakNetClient* gClient = 0;

int Startup()
{
	/*if (!gClient)
	{
		gClient = new gproNet::cRakNetClient();
		if (gClient)
			return 1;
		return 0;
	}*/
	return 0;
}

int Shutdown()
{
	if (gClient)
	{
		delete(gClient);
		gClient = 0;
		return 1;
	}
	return -1;

}

int foo(int bar)
{
	return (bar + 1);
}

int MessageLoop()
{
	gClient->MessageLoop();
	
	return 1;
}

int Login()
{
	return 0;
}

GPRO_NET_SYMBOL char * UpdatePlayerState(float xPosition, float yPosition, float zPosition, char * test)
{
	gClient->state.xPos = xPosition;
	gClient->state.yPos = yPosition;
	gClient->state.zPos = zPosition;
	
	//double xPos = xPosition;
	gClient->sendPlayerState();
	
	return test;
}

const char * GetUsername()
{
	//gClient->GetUsername();
	//char test = (char)gClient->GetUsername();
	return gClient->GetUsername().c_str();
}

GPRO_NET_SYMBOL bool StartClient()
{
		if (!gClient)
		{
			gClient = new gproNet::cRakNetClient();
			if (gClient)
				return true;
			return false;
		}
		return true;
}

GPRO_NET_SYMBOL bool IsActive()
{
	if (gClient)
			return true;
		else
			return false;
}

