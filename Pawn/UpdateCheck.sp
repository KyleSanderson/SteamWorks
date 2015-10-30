#pragma semicolon 1
#include <sourcemod>
#include <SteamWorks>

new g_iPatchVersion = 0;
new g_iAppID = 0;

new Handle:g_hForward = INVALID_HANDLE;

public Plugin:myinfo =
{
    name 			=		"SteamWorks Update Check",				/* https://www.youtube.com/watch?v=Tq_0ht8HCcM */
    author			=		"Kyle Sanderson",
    description		=		"Queries SteamWeb for Updates.",
    version			=		"1.0b",
    url				=		"https://AlliedMods.net"
};

static stock bool:ReadSteamINF(const String:sPath[], &iAppID, &iPatchVersion)
{
	new Handle:hFile = OpenFile(sPath, "r");
	if (hFile == INVALID_HANDLE)
	{
		return false;
	}

	decl String:sBuffer[256];

	do
	{
		if (!ReadFileLine(hFile, sBuffer, sizeof(sBuffer)))
		{
			continue;
		}

		TrimString(sBuffer);
		ReplaceString(sBuffer, sizeof(sBuffer), ".", ""); /* CS:GO uses decimals in steam.inf, WebAPI is Steam| style. */

		new iPos = FindCharInString(sBuffer, '=');
		if (iPos == -1)
		{
			continue;
		}

		sBuffer[iPos++] = '\0';
		switch (CharToLower(sBuffer[0]))
		{
			case 'a':
			{
				if (!StrEqual(sBuffer, "appID", false))
				{
					continue;
				}

				iAppID = StringToInt(sBuffer[iPos]);
			}

			case 'p':
			{
				if (!StrEqual(sBuffer, "PatchVersion", false))
				{
					continue;
				}

				iPatchVersion = StringToInt(sBuffer[iPos]);
			}
		}
	} while (!IsEndOfFile(hFile));

	CloseHandle(hFile);
	return true;
}

public OnPluginStart()
{
	if (!ReadSteamINF("steam.inf", g_iAppID, g_iPatchVersion) && !ReadSteamINF("../steam.inf", g_iAppID, g_iPatchVersion))
	{
		SetFailState("Unable to read steam.inf");
	}

	g_hForward = CreateGlobalForward("SteamWorks_RestartRequested", ET_Ignore);
}

public OnMapStart()
{
	CreateTimer(120.0, OnCheckForUpdate, _, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
}

public Action:OnCheckForUpdate(Handle:hTimer)
{
	static String:sRequest[256];
	if (sRequest[0] == '\0')
	{
		FormatEx(sRequest, sizeof(sRequest), "http://api.steampowered.com/ISteamApps/UpToDateCheck/v0001/?appid=%u&version=%u&format=xml", g_iAppID, g_iPatchVersion);
	}

	new Handle:hRequest = SteamWorks_CreateHTTPRequest(k_EHTTPMethodGET, sRequest);
	if (!hRequest || !SteamWorks_SetHTTPCallbacks(hRequest, OnTransferComplete) || !SteamWorks_SendHTTPRequest(hRequest))
	{
		CloseHandle(hRequest);
	}

	return Plugin_Continue;
}

public OnTransferComplete(Handle:hRequest, bool:bFailure, bool:bRequestSuccessful, EHTTPStatusCode:eStatusCode)
{
	if (!bFailure && bRequestSuccessful && eStatusCode == k_EHTTPStatusCode200OK)
	{
		SteamWorks_GetHTTPResponseBodyCallback(hRequest, APIWebResponse);
	}

	CloseHandle(hRequest);
}

public APIWebResponse(const String:sData[])
{
	new iPos = StrContains(sData, "<required_version>");
	if (iPos == -1)
	{
		return;
	}

	if (g_iPatchVersion != StringToInt(sData[iPos+18]))
	{
		Call_StartForward(g_hForward);
		Call_Finish();
	}
}
