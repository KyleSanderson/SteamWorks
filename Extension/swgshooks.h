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

#pragma once
#include "isteamgameserver.h"
#include "steam_gameserver.h"
#include "smsdk_ext.h"
#include "sourcehook.h"

class SteamWorksGSHooks
{
	public:
		SteamWorksGSHooks();
		~SteamWorksGSHooks();
	
	public:
		void AddHooks(ISteamGameServer *pGameServer);
		void RemoveHooks(ISteamGameServer *pGameServer, bool destroyed = false);
	
	public:
		bool WasRestartRequested(void);
		void LogOnAnonymous(void);
		EBeginAuthSessionResult BeginAuthSession(const void*, int, CSteamID);
		
	private:
		IForward *pFORR; /* On Restart Requested. */
		IForward *pFOTR; /* On Token Requested. */
		IForward *pOBAS; /* On Begin Auth Session. */
		unsigned char uHooked;
};

void OurGameFrameHook(bool simulating);

#include "extension.h"