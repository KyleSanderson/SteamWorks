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

typedef uint32_t Account_t;

class SteamWorksForwards
{
	public:
		SteamWorksForwards();
		~SteamWorksForwards();

	public:
		void NotifyPawnValidateClient(Account_t parent, Account_t child);
		
	private:
		STEAM_GAMESERVER_CALLBACK(SteamWorksForwards, OnGSClientApprove, GSClientApprove_t, m_CallbackGSClientApprove);
		STEAM_GAMESERVER_CALLBACK(SteamWorksForwards, OnValidateTicket, ValidateAuthTicketResponse_t , m_CallbackValidateTicket);
	
	private:
		IForward *pFOVC;
};
