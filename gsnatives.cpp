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

#include "gsnatives.h"

static bool IsSteamWorksLoaded(void)
{
	return (g_SteamWorks.pSWGameServer->GetSteamClient() != NULL);
}

static ISteamGameServer *GetGSPointer(void)
{
	return g_SteamWorks.pSWGameServer->GetGameServer();
}

static cell_t sm_IsVACEnabled(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();
	
	if (pServer == NULL)
	{
		return 0;
	}
	
	return pServer->BSecure() ? 1 : 0;
}

static cell_t sm_GetPublicIP(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();
	
	if (pServer == NULL)
	{
		return 0;
	}
	
	uint32_t ipaddr = pServer->GetPublicIP();
	
	cell_t *addr;
	pContext->LocalToPhysAddr(params[1], &addr);
	for (char iter = 3; iter > -1; --iter)
	{
		addr[(~iter) & 0x03] = (static_cast<unsigned char>(ipaddr >> (iter * 8)) & 0xFF); /* I hate you; SteamTools. */
	}
	
	return 1;
}

static cell_t sm_GetPublicIPCell(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}

	return pServer->GetPublicIP();
}

static cell_t sm_IsLoaded(IPluginContext *pContext, const cell_t *params)
{
	return IsSteamWorksLoaded() ? 1 : 0;
}

static cell_t sm_SetGameDescription(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}
	
	char *pDesc;
	pContext->LocalToString(params[1], &pDesc);
	
	pServer->SetGameDescription(pDesc);
	return 1;
}

static cell_t sm_IsConnected(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}

	return pServer->BLoggedOn() ? 1 : 0;
}

static cell_t sm_SetRule(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}

	char *pKey, *pValue;
	pContext->LocalToString(params[1], &pKey);
	pContext->LocalToString(params[2], &pValue);

	pServer->SetKeyValue(pKey, pValue);
	return 1;
}

static cell_t sm_ClearRules(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}

	pServer->ClearAllKeyValues();
	return 1;
}

static cell_t sm_ForceHeartbeat(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}

	pServer->ForceHeartbeat();
	return 1;
}

static sp_nativeinfo_t gsnatives[] = {
	{"Steam_IsVACEnabled",				sm_IsVACEnabled},
	{"Steam_GetPublicIP",				sm_GetPublicIP},
	{"Steam_GetPublicIPCell",				sm_GetPublicIPCell},
	{"Steam_IsLoaded",				sm_IsLoaded},
	{"Steam_SetGameDescription",	sm_SetGameDescription},
	{"Steam_IsConnected",				sm_IsConnected},
	{"Steam_SetRule",						sm_SetRule},
	{"Steam_ClearRules",						sm_ClearRules},
	{"Steam_ForceHeartbeat",				sm_ForceHeartbeat},
	{NULL,											NULL}
};

SteamWorksGSNatives::SteamWorksGSNatives()
{
	sharesys->AddNatives(myself, gsnatives);
}

SteamWorksGSNatives::~SteamWorksGSNatives()
{
	/* We tragically can't remove ourselves... hopefully no one uses this class, you know, like a class. */
}
