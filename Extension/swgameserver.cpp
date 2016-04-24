/*
    This file is part of SourcePawn SteamWorks.

    SourcePawn SteamWorks is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, as per version 3 of the License.

    SourcePawn SteamWorks is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SourcePawn SteamWorks.  If not, see <http://www.gnu.org/licenses/>.
	
	Author: Kyle Sanderson (KyleS).
*/

#include "swgameserver.h"

static void GetGameSpecificConfigInterface(const char *pName, const char *&pVersion)
{
	if (g_SteamWorks.pSWGameData == NULL)
	{
		return;
	}

	IGameConfig *pConfig = g_SteamWorks.pSWGameData->GetGameData();
	if (pConfig == NULL)
	{
		return;
	}
	
	const char *pNewVersion = pConfig->GetKeyValue(pName);
	if (pNewVersion != NULL)
	{
		pVersion = pNewVersion;
	}
}

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
	this->m_pMatchmaking = NULL;
	this->m_pGC = NULL;
}

ISteamClient *SteamWorksGameServer::GetSteamClient(void)
{
	if (g_pSteamClientGameServer != NULL)
		return g_pSteamClientGameServer;

	/*
		The following is assumed from an unreleased version of the SteamWorks SDK, first seen (and reversed) in CS:GO.
		Thanks CS:GO team! @:|
	*/

	static ISteamClient *pSteamClient = NULL;
	if (pSteamClient == NULL)
	{
		const char *pLibSteamPath = g_SteamWorks.pSWGameServer->GetLibraryPath();

		ILibrary *pLibrary = libsys->OpenLibrary(pLibSteamPath, NULL, 0);
		void *(*pGSInternalCreateAddress)(const char *) = NULL;

		if (pLibrary != NULL)
		{
			const char *pGSInternalFuncName = "SteamGameServerInternal_CreateInterface";

			if (pGSInternalCreateAddress == NULL)
			{
				pGSInternalCreateAddress = reinterpret_cast<void *(*)(const char *)>(pLibrary->GetSymbolAddress(pGSInternalFuncName));
			}

			pLibrary->CloseLibrary();
		}

		if (pGSInternalCreateAddress != NULL)
			pSteamClient = static_cast<ISteamClient *>((*pGSInternalCreateAddress)(STEAMCLIENT_INTERFACE_VERSION));
	}

	return pSteamClient;
}

ISteamGameServer *SteamWorksGameServer::GetGameServer(void)
{
	if (this->m_pGameServer == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);
		
		const char *pVersion = STEAMGAMESERVER_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamGameServerInterfaceVersion", pVersion);
		this->m_pGameServer = this->GetSteamClient()->GetISteamGameServer(hSteamUser, hSteamPipe, pVersion);
	}
	
	return this->m_pGameServer;
}

ISteamUtils *SteamWorksGameServer::GetUtils(void)
{
	if (this->m_pUtils == NULL && this->GetSteamClient() != NULL)
	{
		HSteamPipe hSteamPipe = SteamGameServer_GetHSteamPipe();
		
		const char *pVersion = STEAMUTILS_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamUtilsInterfaceVersion", pVersion);
		this->m_pUtils = this->GetSteamClient()->GetISteamUtils(hSteamPipe, pVersion);
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
		
		const char *pVersion = STEAMNETWORKING_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamNetworkingInterfaceVersion", pVersion);
		this->m_pNetworking = this->GetSteamClient()->GetISteamNetworking(hSteamUser, hSteamPipe, pVersion);
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
		
		const char *pVersion = STEAMGAMESERVERSTATS_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamGameServerStatsInterfaceVersion", pVersion);
		this->m_pStats = this->GetSteamClient()->GetISteamGameServerStats(hSteamUser, hSteamPipe, pVersion);
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
		
		const char *pVersion = STEAMHTTP_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamHTTPInterfaceVersion", pVersion);
		this->m_pHTTP = this->GetSteamClient()->GetISteamHTTP(hSteamUser, hSteamPipe, pVersion);
	}
	
	return this->m_pHTTP;
}

ISteamMatchmaking *SteamWorksGameServer::GetMatchmaking(void)
{
	if (this->m_pMatchmaking == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);
		
		const char *pVersion = STEAMMATCHMAKING_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamMatchmakingVersion", pVersion);
		this->m_pMatchmaking = this->GetSteamClient()->GetISteamMatchmaking(hSteamUser, hSteamPipe, STEAMMATCHMAKING_INTERFACE_VERSION);
	}
	
	return this->m_pMatchmaking;
}

ISteamGameCoordinator *SteamWorksGameServer::GetGameCoordinator(void)
{
	if (this->m_pGC == NULL && this->GetSteamClient() != NULL)
	{
		HSteamUser hSteamUser;
		HSteamPipe hSteamPipe;
		GetUserAndPipe(hSteamUser, hSteamPipe);

		const char *pVersion = STEAMGAMECOORDINATOR_INTERFACE_VERSION;
		GetGameSpecificConfigInterface("SteamGameCoordinatorVersion", pVersion);
		this->m_pGC = static_cast<ISteamGameCoordinator *>(this->GetSteamClient()->GetISteamGenericInterface(hSteamUser, hSteamPipe, STEAMGAMECOORDINATOR_INTERFACE_VERSION));
	}

	return this->m_pGC;
}

void SteamWorksGameServer::GetUserAndPipe(HSteamUser &hSteamUser, HSteamPipe &hSteamPipe)
{
	hSteamUser = SteamGameServer_GetHSteamUser();
	hSteamPipe = SteamGameServer_GetHSteamPipe();
}

const char *SteamWorksGameServer::GetLibraryPath(void)
{
	static const char *pLibSteamPath = NULL;

	if (pLibSteamPath == NULL)
	{
#if defined POSIX
		pLibSteamPath = "./bin/libsteam_api.so";
#elif defined WIN32_LEAN_AND_MEAN
		pLibSteamPath = "./bin/steam_api.dll"; /* Naming from SteamTools. */
#endif

		IGameConfig *pConfig = NULL;
		if (g_SteamWorks.pSWGameData)
		{
			pConfig = g_SteamWorks.pSWGameData->GetGameData();
			if (pConfig)
			{
				const char *kvLibSteamAPI = pConfig->GetKeyValue("LibSteamAPI");
				if (kvLibSteamAPI)
				{
					pLibSteamPath = kvLibSteamAPI;
				}
			}
		}
	}

	return pLibSteamPath;
}
