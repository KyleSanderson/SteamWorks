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

enum
{
	eUnhooked = 0,
	eHooking,
	eHooked
};

SH_DECL_HOOK0(ISteamGameServer, WasRestartRequested, SH_NOATTRIB, 0, bool); /* From SteamTools. */
SH_DECL_HOOK0_void(ISteamGameServer, LogOnAnonymous, SH_NOATTRIB, 0); /* From VoiDeD's MM:S plugin. */

static ISteamGameServer *GetGameServerPointer()
{
	return g_SteamWorks.pSWGameServer->GetGameServer();
}

SteamWorksGSHooks::SteamWorksGSHooks()
{
	this->uHooked = eHooking;
	this->pFORR = forwards->CreateForward("SteamWorks_RestartRequested", ET_Hook, 0, NULL);
	this->pFOTR = forwards->CreateForward("SteamWorks_TokenRequested", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	
	smutils->AddGameFrameHook(OurGameFrameHook);
}

SteamWorksGSHooks::~SteamWorksGSHooks()
{
	this->RemoveHooks(GetGameServerPointer(), true);
	smutils->RemoveGameFrameHook(OurGameFrameHook);
	forwards->ReleaseForward(this->pFORR);
	forwards->ReleaseForward(this->pFOTR);
}

void SteamWorksGSHooks::LogOnAnonymous(void)
{
	ISteamGameServer *pGameServer = GetGameServerPointer();
	if (pGameServer == NULL)
	{
		/* Go away, this wrecks us if we want to use it later. Also; impossible. */
		RETURN_META(MRES_SUPERCEDE);
	}

	char pToken[256];
	pToken[0] = '\0';

	if (this->pFOTR->GetFunctionCount() != 0)
	{
		cell_t result = Pl_Continue;
		this->pFOTR->PushStringEx(pToken, sizeof(pToken), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
		this->pFOTR->PushCell(sizeof(pToken));
		this->pFOTR->Execute(&result);
	}

	pGameServer->LogOn("", pToken);
	RETURN_META(MRES_SUPERCEDE);
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
	if (this->uHooked == eHooked || pGameServer == NULL)
	{
		return;
	}

	this->uHooked = eHooked;
	SH_ADD_HOOK(ISteamGameServer, WasRestartRequested, pGameServer, SH_MEMBER(this, &SteamWorksGSHooks::WasRestartRequested), false);
	SH_ADD_HOOK(ISteamGameServer, LogOnAnonymous, pGameServer, SH_MEMBER(this, &SteamWorksGSHooks::LogOnAnonymous), false);
}

void SteamWorksGSHooks::RemoveHooks(ISteamGameServer *pGameServer, bool destroyed)
{
	if (this->uHooked != eHooked || pGameServer == NULL)
	{
		return;
	}

	SH_REMOVE_HOOK(ISteamGameServer, WasRestartRequested, pGameServer, SH_MEMBER(this, &SteamWorksGSHooks::WasRestartRequested), false);
	SH_REMOVE_HOOK(ISteamGameServer, LogOnAnonymous, pGameServer, SH_MEMBER(this, &SteamWorksGSHooks::LogOnAnonymous), false);
	if (destroyed)
	{
		this->uHooked = eUnhooked;
		return;
	}

	this->uHooked = eHooking;
	smutils->AddGameFrameHook(OurGameFrameHook);
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
