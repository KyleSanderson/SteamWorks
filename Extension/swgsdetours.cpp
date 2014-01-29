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

#include "swgsdetours.h"

DETOUR_DECL_STATIC0(SteamAPIShutdown, void)
{
	DETOUR_STATIC_CALL(SteamAPIShutdown)(); /* We're not a monster. */
	if (g_SteamWorks.pSWGameServer == NULL)
	{
		return;
	}
	
	g_SteamWorks.pSWGameServer->Reset();
}

SteamWorksGSDetours::SteamWorksGSDetours()
{
#if defined POSIX
	const char *pLibSteamPath = "./bin/libsteam_api.so";
#elif defined WIN32_LEAN_AND_MEAN
	const char *pLibSteamPath = "./bin/steam_api.dll"; /* Naming from SteamTools. */
#endif

	IGameConfig *pConfig = NULL;
	if (g_SteamWorks.pSWGameData)
	{
		pConfig = g_SteamWorks.pSWGameData->GetGameData();
		if (pConfig)
		{
			pLibSteamPath = pConfig->GetKeyValue("LibSteamAPI");
		}
	}

	ILibrary *pLibrary = libsys->OpenLibrary(pLibSteamPath, NULL, 0);
	void *pAddress = NULL;
	if (pLibrary != NULL)
	{
		const char *pFunctionName = "SteamGameServer_Shutdown";
		if (pConfig == NULL || pConfig->GetMemSig(pFunctionName, &pAddress) == false)
		{
			pAddress = pLibrary->GetSymbolAddress(pFunctionName);
		}
		
		pLibrary->CloseLibrary();
	}

	if (pAddress == NULL)
	{
		this->m_pShutdownDetour = NULL;
		return;
	}

	CDetourManager::Init(g_pSM->GetScriptingEngine(), pConfig);
	this->m_pShutdownDetour = DETOUR_CREATE_STATIC_FIXED(SteamAPIShutdown, pAddress);
	if (this->m_pShutdownDetour != NULL)
	{
		this->m_pShutdownDetour->EnableDetour();
	}
}

SteamWorksGSDetours::~SteamWorksGSDetours()
{
	if (this->m_pShutdownDetour != NULL)
	{
		this->m_pShutdownDetour->Destroy();
		this->m_pShutdownDetour = NULL;
	}
}
