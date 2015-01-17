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

#include "swgsdetours.h"

DETOUR_DECL_STATIC0(SteamAPIShutdown, void)
{
	if (g_SteamWorks.pSWGameServer != NULL)
	{
		if (g_SteamWorks.pGSHooks != NULL)
		{
			g_SteamWorks.pGSHooks->RemoveHooks(g_SteamWorks.pSWGameServer->GetGameServer(), false);
		}
		
		g_SteamWorks.pSWGameServer->Reset();
	}

	DETOUR_STATIC_CALL(SteamAPIShutdown)(); /* We're not a monster. */
}

DETOUR_DECL_STATIC6(SteamGameServer_InitSafeDetour, bool, uint32, unIP, uint16, usSteamPort, uint16, usGamePort, uint16, usQueryPort, EServerMode, eServerMode, const char *, pchVersionString)
{
	bool bRet = DETOUR_STATIC_CALL(SteamGameServer_InitSafeDetour)(unIP, usSteamPort, usGamePort, usQueryPort, eServerMode, pchVersionString); /* Call to init game interfaces. */
	
	if (g_SteamWorks.pSWGameServer != NULL && g_SteamWorks.pGSHooks != NULL)
	{
		g_SteamWorks.pGSHooks->AddHooks(g_SteamWorks.pSWGameServer->GetGameServer());
	}
	
	return bRet;
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
			const char *kvLibSteamAPI = pConfig->GetKeyValue("LibSteamAPI");
			if (kvLibSteamAPI)
			{
				pLibSteamPath = kvLibSteamAPI;
			}
		}
	}

	ILibrary *pLibrary = libsys->OpenLibrary(pLibSteamPath, NULL, 0);
	void *pSteamSafeInitAddress = NULL;
	void *pSteamShutdownAddress = NULL;
	if (pLibrary != NULL)
	{
		const char *pInitSafeFuncName = "SteamGameServer_InitSafe";
		const char *pShutdownFuncName = "SteamGameServer_Shutdown";
		if (pConfig != NULL)
		{
			pConfig->GetMemSig(pShutdownFuncName, &pSteamShutdownAddress);
			pConfig->GetMemSig(pInitSafeFuncName, &pSteamSafeInitAddress);
		}
		
		if (pSteamShutdownAddress == NULL)
		{
			pSteamShutdownAddress = pLibrary->GetSymbolAddress(pShutdownFuncName);
		}
		
		if (pSteamSafeInitAddress == NULL)
		{
			pSteamSafeInitAddress = pLibrary->GetSymbolAddress(pInitSafeFuncName);
		}
		
		pLibrary->CloseLibrary();
	}

	CDetourManager::Init(g_pSM->GetScriptingEngine(), pConfig);
	if (pSteamShutdownAddress != NULL)
	{
		this->m_pShutdownDetour = DETOUR_CREATE_STATIC_FIXED(SteamAPIShutdown, pSteamShutdownAddress);
		this->m_pShutdownDetour->EnableDetour();
	}
	else
	{
		this->m_pShutdownDetour = NULL;
	}

	if (pSteamSafeInitAddress != NULL)
	{
		this->m_pSafeInitDetour = DETOUR_CREATE_STATIC_FIXED(SteamGameServer_InitSafeDetour, pSteamSafeInitAddress);
		this->m_pSafeInitDetour->EnableDetour();
	}
	else
	{
		this->m_pSafeInitDetour = NULL;
	}
}

SteamWorksGSDetours::~SteamWorksGSDetours()
{
	if (this->m_pShutdownDetour != NULL)
	{
		this->m_pShutdownDetour->Destroy();
		this->m_pShutdownDetour = NULL;
	}
	
	if (this->m_pSafeInitDetour != NULL)
	{
		this->m_pSafeInitDetour->Destroy();
		this->m_pSafeInitDetour = NULL;
	}
}
