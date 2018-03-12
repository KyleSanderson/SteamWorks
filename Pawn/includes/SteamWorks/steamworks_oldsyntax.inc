native bool:SteamWorks_IsVACEnabled();
native bool:SteamWorks_GetPublicIP(ipaddr[4]);
native SteamWorks_GetPublicIPCell();
native bool:SteamWorks_IsLoaded();
native bool:SteamWorks_SetGameDescription(const String:sDesc[]);
native bool:SteamWorks_SetMapName(const String:sMapName[]);
native bool:SteamWorks_IsConnected();
native bool:SteamWorks_SetRule(const String:sKey[], const String:sValue[]);
native bool:SteamWorks_ClearRules();
native bool:SteamWorks_ForceHeartbeat();
native bool:SteamWorks_GetUserGroupStatus(client, groupid);
native bool:SteamWorks_GetUserGroupStatusAuthID(authid, groupid);

native EUserHasLicenseForAppResult:SteamWorks_HasLicenseForApp(client, app);
native EUserHasLicenseForAppResult:SteamWorks_HasLicenseForAppId(authid, app);
native SteamWorks_GetClientSteamID(client, String:sSteamID[], length);

native bool:SteamWorks_RequestStatsAuthID(authid, appid);
native bool:SteamWorks_RequestStats(client, appid);
native bool:SteamWorks_GetStatCell(client, const String:sKey[], &value);
native bool:SteamWorks_GetStatAuthIDCell(authid, const String:sKey[], &value);
native bool:SteamWorks_GetStatFloat(client, const String:sKey[], &Float:value);
native bool:SteamWorks_GetStatAuthIDFloat(authid, const String:sKey[], &Float:value);

native Handle:SteamWorks_CreateHTTPRequest(EHTTPMethod:method, const String:sURL[]);
native bool:SteamWorks_SetHTTPRequestContextValue(Handle:hHandle, any:data1, any:data2=0);
native bool:SteamWorks_SetHTTPRequestNetworkActivityTimeout(Handle:hHandle, timeout);
native bool:SteamWorks_SetHTTPRequestHeaderValue(Handle:hHandle, const String:sName[], const String:sValue[]);
native bool:SteamWorks_SetHTTPRequestGetOrPostParameter(Handle:hHandle, const String:sName[], const String:sValue[]);
native bool:SteamWorks_SetHTTPRequestUserAgentInfo(Handle:hHandle, const String:sUserAgentInfo[]);
native bool:SteamWorks_SetHTTPRequestRequiresVerifiedCertificate(Handle:hHandle, bool:bRequireVerifiedCertificate);
native bool:SteamWorks_SetHTTPRequestAbsoluteTimeoutMS(Handle:hHandle, unMilliseconds);


funcenum SteamWorksHTTPRequestCompleted
{
	public(Handle:hRequest, bool:bFailure, bool:bRequestSuccessful, EHTTPStatusCode:eStatusCode),
	public(Handle:hRequest, bool:bFailure, bool:bRequestSuccessful, EHTTPStatusCode:eStatusCode, any:data1),
	public(Handle:hRequest, bool:bFailure, bool:bRequestSuccessful, EHTTPStatusCode:eStatusCode, any:data1, any:data2)
};

funcenum SteamWorksHTTPHeadersReceived
{
	public(Handle:hRequest, bool:bFailure),
	public(Handle:hRequest, bool:bFailure, any:data1),
	public(Handle:hRequest, bool:bFailure, any:data1, any:data2)
};

funcenum SteamWorksHTTPDataReceived
{
	public(Handle:hRequest, bool:bFailure, offset, bytesreceived),
	public(Handle:hRequest, bool:bFailure, offset, bytesreceived, any:data1),
	public(Handle:hRequest, bool:bFailure, offset, bytesreceived, any:data1, any:data2)
};

native bool:SteamWorks_SetHTTPCallbacks(Handle:hHandle, SteamWorksHTTPRequestCompleted:fCompleted = INVALID_FUNCTION, SteamWorksHTTPHeadersReceived:fHeaders = INVALID_FUNCTION, SteamWorksHTTPDataReceived:fData = INVALID_FUNCTION, Handle:hCalling = INVALID_HANDLE);
native bool:SteamWorks_SendHTTPRequest(Handle:hRequest);
native bool:SteamWorks_SendHTTPRequestAndStreamResponse(Handle:hRequest);
native bool:SteamWorks_DeferHTTPRequest(Handle:hRequest);
native bool:SteamWorks_PrioritizeHTTPRequest(Handle:hRequest);
native bool:SteamWorks_GetHTTPResponseHeaderSize(Handle:hRequest, const String:sHeader[], &size);
native bool:SteamWorks_GetHTTPResponseHeaderValue(Handle:hRequest, const String:sHeader[], String:sValue[], size);
native bool:SteamWorks_GetHTTPResponseBodySize(Handle:hRequest, &size);
native bool:SteamWorks_GetHTTPResponseBodyData(Handle:hRequest, String:sBody[], length);
native bool:SteamWorks_GetHTTPStreamingResponseBodyData(Handle:hRequest, cOffset, String:sBody[], length);
native bool:SteamWorks_GetHTTPDownloadProgressPct(Handle:hRequest, &Float:percent);
native bool:SteamWorks_GetHTTPRequestWasTimedOut(Handle:hRequest, &bool:bWasTimedOut);
native bool:SteamWorks_SetHTTPRequestRawPostBody(Handle:hRequest, const String:sContentType[], const String:sBody[], bodylen);

funcenum SteamWorksHTTPBodyCallback
{
	public(const String:sData[]),
	public(const String:sData[], any:value),
	public(const data[], any:value, datalen)
};

native bool:SteamWorks_GetHTTPResponseBodyCallback(Handle:hRequest, SteamWorksHTTPBodyCallback:fCallback, any:data = 0, Handle:hPlugin = INVALID_HANDLE);
native bool:SteamWorks_WriteHTTPResponseBodyToFile(Handle:hRequest, const String:sFileName[]);

forward SW_OnValidateClient(ownerauthid, authid);
forward SteamWorks_OnValidateClient(ownerauthid, authid);
forward SteamWorks_SteamServersConnected();
forward SteamWorks_SteamServersConnectFailure(EResult:result);
forward SteamWorks_SteamServersDisconnected(EResult:result);

forward Action:SteamWorks_RestartRequested();
forward SteamWorks_TokenRequested(String:sToken[], maxlen);

forward SteamWorks_OnClientGroupStatus(authid, groupid, bool:isMember, bool:isOfficer);

forward EGCResults:SteamWorks_GCSendMessage(unMsgType, const String:pubData[], cubData);
forward SteamWorks_GCMsgAvailable(cubData);
forward EGCResults:SteamWorks_GCRetrieveMessage(punMsgType, const String:pubDest[], cubDest, pcubMsgSize);

native EGCResults:SteamWorks_SendMessageToGC(unMsgType, const String:pubData[], cubData);