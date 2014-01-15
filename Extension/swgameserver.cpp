/*
    This file is part of SourcePawn SteamWorks.

    SourcePawn SteamWorks is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SourcePawn SteamWorks is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SourcePawn SteamWorks.  If not, see <http://www.gnu.org/licenses/>.
	
	Author: Kyle Sanderson (KyleS).
*/

#include "swgameserver.h"

SteamWorksGameServer::SteamWorksGameServer()
{
	this->Reset();
}

SteamWorksGameServer::~SteamWorksGameServer()
{
}

void SteamWorksGameServer::Reset(void)
{
	this->m_pGameServer = NULL;
	this->m_pUtils = NULL;
	this->m_pNetworking = NULL;
	this->m_pStats = NULL;
	this->m_pHTTP = NULL;
}

ISteamClient *SteamWorksGameServer::GetSteamClient(void)
{
	return g_pSteamClientGameServer;
}

ISteamGameServer *SteamWorksGameServer::GetGameServer(void)
{
	if (this->m_pGameServer == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);
		
		this->m_pGameServer = this->GetSteamClient()->GetISteamGameServer(hSteamUser, hSteamPipe, STEAMGAMESERVER_INTERFACE_VERSION);
	}
	
	return this->m_pGameServer;
}

ISteamUtils *SteamWorksGameServer::GetUtils(void)
{
	if (this->m_pUtils == NULL && this->GetSteamClient() != NULL)
	{
		HSteamPipe hSteamPipe = SteamGameServer_GetHSteamPipe();
		
		this->m_pUtils = this->GetSteamClient()->GetISteamUtils(hSteamPipe, STEAMUTILS_INTERFACE_VERSION);
	}
	
	return this->m_pUtils;
}

ISteamNetworking *SteamWorksGameServer::GetNetworking(void)
{
	if (this->m_pNetworking == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);
		
		this->m_pNetworking = this->GetSteamClient()->GetISteamNetworking(hSteamUser, hSteamPipe, STEAMNETWORKING_INTERFACE_VERSION);
	}
	
	return this->m_pNetworking;
}

ISteamGameServerStats *SteamWorksGameServer::GetServerStats(void)
{
	if (this->m_pStats == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);
		
		this->m_pStats = this->GetSteamClient()->GetISteamGameServerStats(hSteamUser, hSteamPipe, STEAMGAMESERVERSTATS_INTERFACE_VERSION);
	}
	
	return this->m_pStats;
}

ISteamHTTP *SteamWorksGameServer::GetHTTP(void)
{
	if (this->m_pHTTP == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);
		
		this->m_pHTTP = this->GetSteamClient()->GetISteamHTTP(hSteamUser, hSteamPipe, STEAMHTTP_INTERFACE_VERSION);
	}
	
	return this->m_pHTTP;
}

void SteamWorksGameServer::GetUserAndPipe(HSteamUser &hSteamUser, HSteamPipe &hSteamPipe)
{
	hSteamUser = SteamGameServer_GetHSteamUser();
	hSteamPipe = SteamGameServer_GetHSteamPipe();
}
