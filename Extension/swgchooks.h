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
#include "steam_gameserver.h"
#include "isteamgamecoordinator.h"

#include "smsdk_ext.h"
#include "sourcehook.h"

class SteamWorksGCHooks
{
	public:
		SteamWorksGCHooks();
		~SteamWorksGCHooks();

	public:
		void AddHooks(ISteamGameCoordinator *pGC);
		void RemoveHooks(ISteamGameCoordinator *pGC, bool destroyed = false);

	public:
		EGCResults SendMessage(uint32 unMsgType, const void *pubData, uint32 cubData);
		bool IsMessageAvailable(uint32_t *pcubMsgSize);
		EGCResults RetrieveMessage(uint32 *punMsgType, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize);

	private:
		IForward *pGCSendMsg;
		IForward *pGCMsgAvail;
		IForward *pGCRetMsg;
		unsigned char uHooked;
};

void OurGCGameFrameHook(bool simulating);

#include "extension.h"
