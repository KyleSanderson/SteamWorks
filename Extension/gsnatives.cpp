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

#include "gsnatives.h"

static bool IsSteamWorksLoaded(void)
{
	return (g_SteamWorks.pSWGameServer->GetSteamClient() != NULL);
}

static ISteamGameServer *GetGSPointer(void)
{
	return g_SteamWorks.pSWGameServer->GetGameServer();
}

static CSteamID CreateCommonCSteamID(IGamePlayer *pPlayer, const cell_t *params, unsigned char universeplace = 2, unsigned char typeplace = 3)
{
	return g_SteamWorks.CreateCommonCSteamID(pPlayer, params, universeplace, typeplace);
}

static CSteamID CreateCommonCSteamID(uint32_t authid, const cell_t *params, unsigned char universeplace = 2, unsigned char typeplace = 3)
{
	return g_SteamWorks.CreateCommonCSteamID(authid, params, universeplace, typeplace);
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

static cell_t sm_SetGameData(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}
	
	char *pData;
	pContext->LocalToString(params[1], &pData);
	
	pServer->SetGameData(pData);
	return 1;
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

static cell_t sm_SetMapName(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return 0;
	}
	
	char *pMapName;
	pContext->LocalToString(params[1], &pMapName);
	
	pServer->SetMapName(pMapName);
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

static cell_t sm_UserHasLicenseForApp(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return k_EUserHasLicenseResultNoAuth;
	}
	
	int client = gamehelpers->ReferenceToIndex(params[1]);
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client); /* Man, including GameHelpers and PlayerHelpers for this native :(. */
	if (pPlayer == NULL || pPlayer->IsConnected() == false)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	
	CSteamID checkid = CreateCommonCSteamID(pPlayer, params, 3, 4);
	return pServer->UserHasLicenseForApp(checkid, params[2]);
}

static cell_t sm_UserHasLicenseForAppId(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return k_EUserHasLicenseResultNoAuth;
	}

	CSteamID checkid = CreateCommonCSteamID(params[1], params, 3, 4);
	return pServer->UserHasLicenseForApp(checkid, params[2]);
}

static cell_t sm_GetClientSteamID(IPluginContext *pContext, const cell_t *params)
{
	int client = gamehelpers->ReferenceToIndex(params[1]);
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client);

	if (pPlayer == NULL || pPlayer->IsConnected() == false)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	CSteamID steamId = CreateCommonCSteamID(pPlayer, params, 4, 5);

	char *steamIdBuffer;
	pContext->LocalToString(params[2], &steamIdBuffer);

	int numBytes = g_pSM->Format(steamIdBuffer, params[3], "%llu", steamId.ConvertToUint64());
	numBytes++; // account for null terminator
	
	return numBytes;
}

static cell_t sm_GetUserGroupStatus(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return false;
	}

	int client = gamehelpers->ReferenceToIndex(params[1]);
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client); /* Man, including GameHelpers and PlayerHelpers for this native :(. */
	if (pPlayer == NULL || pPlayer->IsConnected() == false)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	CSteamID checkid = CreateCommonCSteamID(pPlayer, params, 3, 4);
	return pServer->RequestUserGroupStatus(checkid, CSteamID(params[2], k_EUniversePublic, k_EAccountTypeClan));
}

static cell_t sm_GetUserGroupStatusAuthID(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServer *pServer = GetGSPointer();

	if (pServer == NULL)
	{
		return false;
	}

	CSteamID checkid = CreateCommonCSteamID(params[1], params, 3, 4);
	return pServer->RequestUserGroupStatus(checkid, CSteamID(params[2], k_EUniversePublic, k_EAccountTypeClan));
}

static sp_nativeinfo_t gsnatives[] = {
	{"SteamWorks_IsVACEnabled",				sm_IsVACEnabled},
	{"SteamWorks_GetPublicIP",				sm_GetPublicIP},
	{"SteamWorks_GetPublicIPCell",				sm_GetPublicIPCell},
	{"SteamWorks_IsLoaded",				sm_IsLoaded},
	{"SteamWorks_SetGameData",				sm_SetGameData},
	{"SteamWorks_SetGameDescription",	sm_SetGameDescription},
	{"SteamWorks_SetMapName",	sm_SetMapName},
	{"SteamWorks_IsConnected",				sm_IsConnected},
	{"SteamWorks_SetRule",						sm_SetRule},
	{"SteamWorks_ClearRules",						sm_ClearRules},
	{"SteamWorks_ForceHeartbeat",				sm_ForceHeartbeat},
	{"SteamWorks_HasLicenseForApp",			sm_UserHasLicenseForApp},
	{"SteamWorks_HasLicenseForAppId",			sm_UserHasLicenseForAppId},
	{"SteamWorks_GetClientSteamID",			sm_GetClientSteamID},
	{"SteamWorks_GetUserGroupStatus",			sm_GetUserGroupStatus},
	{"SteamWorks_GetUserGroupStatusAuthID",			sm_GetUserGroupStatusAuthID},
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
