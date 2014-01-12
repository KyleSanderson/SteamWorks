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

#include "swgshooks.h"

SH_DECL_HOOK0(ISteamGameServer, WasRestartRequested, SH_NOATTRIB, 0, bool); /* From SteamTools. */

static ISteamGameServer *GetGameServerPointer()
{
	return g_SteamWorks.pSWGameServer->GetGameServer();
}

SteamWorksGSHooks::SteamWorksGSHooks()
{
	this->bHooked = false;
	this->pFORR = forwards->CreateForward("SteamWorks_RestartRequested", ET_Hook, 0, NULL);
	
	smutils->AddGameFrameHook(OurGameFrameHook);
}

SteamWorksGSHooks::~SteamWorksGSHooks()
{
	smutils->RemoveGameFrameHook(OurGameFrameHook);

	this->RemoveHooks(GetGameServerPointer());
	forwards->ReleaseForward(this->pFORR);
}

bool SteamWorksGSHooks::WasRestartRequested(void) /* Mimic SteamTools. */
{
	bool bWasRestartRequested = SH_CALL(GetGameServerPointer(), &ISteamGameServer::WasRestartRequested)();
	if (bWasRestartRequested && this->pFORR->GetFunctionCount() != 0)
	{
		cell_t Result = Pl_Continue;
		this->pFORR->Execute(&Result);
		bWasRestartRequested = (Result >= Pl_Handled);
	}

	/* With how this function works, all following will be given poisoned values from SH_Call. */
	RETURN_META_VALUE(MRES_SUPERCEDE, bWasRestartRequested); 
}

void SteamWorksGSHooks::AddHooks(ISteamGameServer *pGameServer)
{
	if (this->bHooked == true || pGameServer == NULL)
	{
		return;
	}

	this->bHooked = true;
	SH_ADD_HOOK(ISteamGameServer, WasRestartRequested, pGameServer, SH_MEMBER(this, &SteamWorksGSHooks::WasRestartRequested), false);
}

void SteamWorksGSHooks::RemoveHooks(ISteamGameServer *pGameServer)
{
	if (this->bHooked == false || pGameServer == NULL)
	{
		return;
	}

	this->bHooked = false;
	SH_REMOVE_HOOK(ISteamGameServer, WasRestartRequested, pGameServer, SH_MEMBER(this, &SteamWorksGSHooks::WasRestartRequested), false);
}

void OurGameFrameHook(bool simulating) /* What we do for SDK independence. */
{
	ISteamGameServer *pGameServer = GetGameServerPointer();
	if (pGameServer == NULL)
	{
		return;
	}

	g_SteamWorks.pGSHooks->AddHooks(pGameServer);
	smutils->RemoveGameFrameHook(OurGameFrameHook);
}
