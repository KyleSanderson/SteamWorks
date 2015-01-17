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

#pragma once
#include "smsdk_ext.h"
#include "steam_gameserver.h"

class SteamWorksGameServer
{
	public:
		SteamWorksGameServer();
		~SteamWorksGameServer();

	public:
		ISteamClient *GetSteamClient(void);
		ISteamGameServer *GetGameServer(void);
		ISteamUtils *GetUtils(void);
		ISteamNetworking *GetNetworking(void);
		ISteamGameServerStats *GetServerStats(void);
		ISteamHTTP *GetHTTP(void);
		ISteamMatchmaking *GetMatchmaking(void);
	public:
		void Reset(void);
	private:
		void GetUserAndPipe(HSteamUser &hSteamUser, HSteamPipe &hSteamPipe);
	private:
		ISteamClient *m_pClient;
		ISteamGameServer *m_pGameServer;
		ISteamUtils *m_pUtils;
		ISteamNetworking *m_pNetworking;
		ISteamGameServerStats *m_pStats;
		ISteamHTTP *m_pHTTP;
		ISteamMatchmaking *m_pMatchmaking;
		bool loaded;
};

#include "extension.h"
