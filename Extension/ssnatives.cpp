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

#include "ssnatives.h"

static bool IsSteamWorksLoaded(void)
{
	return (g_SteamWorks.pSWGameServer->GetSteamClient() != NULL);
}

static ISteamGameServerStats *GetServerStatsPointer(void)
{
	return g_SteamWorks.pSWGameServer->GetServerStats();
}

static CSteamID CreateCommonCSteamID(IGamePlayer *pPlayer, const cell_t *params, unsigned char universeplace = 2, unsigned char typeplace = 3)
{
	return g_SteamWorks.CreateCommonCSteamID(pPlayer, params, universeplace, typeplace);
}

static CSteamID CreateCommonCSteamID(uint32_t authid, const cell_t *params, unsigned char universeplace = 2, unsigned char typeplace = 3)
{
	return g_SteamWorks.CreateCommonCSteamID(authid, params, universeplace, typeplace);
}

static cell_t sm_RequestStatsAuthID(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServerStats *pStats = GetServerStatsPointer();
	
	if (pStats == NULL)
	{
		return 0;
	}

	CSteamID checkid = CreateCommonCSteamID(params[1], params);
	return pStats->RequestUserStats(checkid) != k_uAPICallInvalid ? 1 : 0;
}

static cell_t sm_RequestUserStats(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServerStats *pStats = GetServerStatsPointer();

	if (pStats == NULL)
	{
		return 0;
	}

	int client = gamehelpers->ReferenceToIndex(params[1]);
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client); /* Man, including GameHelpers and PlayerHelpers for this native :(. */
	if (pPlayer == NULL || pPlayer->IsConnected() == false)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	CSteamID checkid = CreateCommonCSteamID(pPlayer, params);
	return pStats->RequestUserStats(checkid) != k_uAPICallInvalid ? 1 : 0;
}

static cell_t sm_GetStatCell(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServerStats *pStats = GetServerStatsPointer();

	if (pStats == NULL)
	{
		return 0;
	}

	int client = gamehelpers->ReferenceToIndex(params[1]);
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client); /* Man, including GameHelpers and PlayerHelpers for this native :(. */
	if (pPlayer == NULL || pPlayer->IsConnected() == false)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	
	char *pName;
	pContext->LocalToString(params[2], &pName);

	cell_t *pValue;
	pContext->LocalToPhysAddr(params[3], &pValue);
	CSteamID checkid = CreateCommonCSteamID(pPlayer, params, 4, 5);
	return pStats->GetUserStat(checkid, pName, pValue) ? 1 : 0;
}

static cell_t sm_GetStatAuthIDCell(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServerStats *pStats = GetServerStatsPointer();

	if (pStats == NULL)
	{
		return 0;
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);

	cell_t *pValue;
	pContext->LocalToPhysAddr(params[3], &pValue);
	CSteamID checkid = CreateCommonCSteamID(params[1], params, 4, 5);
	return pStats->GetUserStat(checkid, pName, pValue) ? 1 : 0;
}

static cell_t sm_GetStatFloat(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServerStats *pStats = GetServerStatsPointer();

	if (pStats == NULL)
	{
		return 0;
	}

	int client = gamehelpers->ReferenceToIndex(params[1]);
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client); /* Man, including GameHelpers and PlayerHelpers for this native :(. */
	if (pPlayer == NULL || pPlayer->IsConnected() == false)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	
	char *pName;
	pContext->LocalToString(params[2], &pName);

	cell_t *pValue;
	pContext->LocalToPhysAddr(params[3], &pValue);
	CSteamID checkid = CreateCommonCSteamID(pPlayer, params, 4, 5);
	
	float fValue;
	bool bResult = pStats->GetUserStat(checkid, pName, &fValue);
	
	*pValue = sp_ftoc(fValue);
	
	return bResult ? 1 : 0;
}

static cell_t sm_GetStatAuthIDFloat(IPluginContext *pContext, const cell_t *params)
{
	ISteamGameServerStats *pStats = GetServerStatsPointer();

	if (pStats == NULL)
	{
		return 0;
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);

	cell_t *pValue;
	pContext->LocalToPhysAddr(params[3], &pValue);
	CSteamID checkid = CreateCommonCSteamID(params[1], params, 4, 5);

	float fValue;
	bool bResult = pStats->GetUserStat(checkid, pName, &fValue);

	*pValue = sp_ftoc(fValue);

	return bResult ? 1 : 0;
}

static sp_nativeinfo_t ssnatives[] = {
	{"SteamWorks_RequestStatsAuthID",				sm_RequestStatsAuthID},
	{"SteamWorks_RequestStats",				sm_RequestUserStats},
	{"SteamWorks_GetStatCell",				sm_GetStatCell},
	{"SteamWorks_GetStatAuthIDCell",				sm_GetStatAuthIDCell},
	{"SteamWorks_GetStatFloat",				sm_GetStatFloat},
	{"SteamWorks_GetStatAuthIDFloat",				sm_GetStatAuthIDFloat},
	{NULL,											NULL}
};

SteamWorksSSNatives::SteamWorksSSNatives()
{
	sharesys->AddNatives(myself, ssnatives);
}

SteamWorksSSNatives::~SteamWorksSSNatives()
{
	/* We tragically can't remove ourselves... hopefully no one uses this class, you know, like a class. */
}
