/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include <stdlib.h>

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

SteamWorks g_SteamWorks;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_SteamWorks);
 
bool SteamWorks::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	sharesys->RegisterLibrary(myself, "SteamWorks");

	this->pSWGameData = new SteamWorksGameData;
	this->pSWGameServer = new SteamWorksGameServer;
	this->pSWHTTP = new SteamWorksHTTP;
	
	this->pSWHTTPNatives = new SteamWorksHTTPNatives;
	this->pSWForward = new SteamWorksForwards;
	this->pGSNatives = new SteamWorksGSNatives;
	this->pGSHooks = new SteamWorksGSHooks;
	this->pGSDetours = new SteamWorksGSDetours;
	this->pSSNatives = new SteamWorksSSNatives;
	return true;
}

void SteamWorks::SDK_OnUnload()
{
	delete this->pSSNatives;
	delete this->pGSDetours;
	delete this->pGSHooks;
	delete this->pGSNatives;
	delete this->pSWForward;
	delete this->pSWHTTPNatives;
	
	delete this->pSWHTTP;
	delete this->pSWGameServer;
	delete this->pSWGameData;
}

bool SteamWorks::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetFileSystemFactory, this->pFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);

	return true;
}

bool SteamWorks::SDK_OnMetamodUnload(char *error, size_t maxlen)
{
	this->pFileSystem = NULL;

	return true;
}

CSteamID SteamWorks::CreateCommonCSteamID(IGamePlayer *pPlayer, const cell_t *params, unsigned char universeplace = 2, unsigned char typeplace = 3)
{
	EUniverse universe = k_EUniversePublic;
	EAccountType type = k_EAccountTypeIndividual;
	
	const char *pAuth = pPlayer->GetAuthString(false); /* We're not using this for Auth. */
	if (pAuth == NULL || pAuth[0] == '\0' || strlen(pAuth) < 11 || pAuth[6] == 'I')
	{
		return this->CreateCommonCSteamID(pPlayer->GetSteamAccountID(false), params, universeplace, typeplace);
	}

	universe = static_cast<EUniverse>(atoi(&pAuth[6]));
	if (universe == k_EUniverseInvalid)
	{
		universe = k_EUniversePublic; /* Legacy Engine shim. */
	}

	return CSteamID(pPlayer->GetSteamAccountID(false), universe, type);
}

CSteamID SteamWorks::CreateCommonCSteamID(uint32_t authid, const cell_t *params, unsigned char universeplace = 2, unsigned char typeplace = 3)
{
	EUniverse universe = k_EUniversePublic;
	EAccountType type = k_EAccountTypeIndividual;
	if (params[0] >= universeplace)
	{
		universe = static_cast<EUniverse>(params[universeplace]);
	}

	if (params[0] >= typeplace)
	{
		type = static_cast<EAccountType>(params[typeplace]);
	}

	return CSteamID(authid, universe, type);
}
