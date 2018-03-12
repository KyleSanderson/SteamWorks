native bool SteamWorks_IsVACEnabled();
native bool SteamWorks_GetPublicIP(int ipaddr[4]);
native int SteamWorks_GetPublicIPCell();
native bool SteamWorks_IsLoaded();
native bool SteamWorks_SetGameDescription(const char[] sDesc);
native bool SteamWorks_SetMapName(const char[] sMapName);
native bool SteamWorks_IsConnected();
native bool SteamWorks_SetRule(const char[] sKey, const char[] sValue);
native bool SteamWorks_ClearRules();
native bool SteamWorks_ForceHeartbeat();
native bool SteamWorks_GetUserGroupStatus(int client, int groupid);
native bool SteamWorks_GetUserGroupStatusAuthID(int authid, int groupid);

native EUserHasLicenseForAppResult SteamWorks_HasLicenseForApp(int client, int app);
native EUserHasLicenseForAppResult SteamWorks_HasLicenseForAppId(int authid, int app);
native int SteamWorks_GetClientSteamID(int client, char[] sSteamID, int length);

native bool SteamWorks_RequestStatsAuthID(int authid, int appid);
native bool SteamWorks_RequestStats(int client, int appid);
native bool SteamWorks_GetStatCell(int client, const char[] sKey, int &value);
native bool SteamWorks_GetStatAuthIDCell(int authid, const char[] sKey, int &value);
native bool SteamWorks_GetStatFloat(int client, const char[] sKey, float &value);
native bool SteamWorks_GetStatAuthIDFloat(int authid, const char[] sKey, float &value);

native Handle SteamWorks_CreateHTTPRequest(EHTTPMethod method, const char[] sURL);
native bool SteamWorks_SetHTTPRequestContextValue(Handle hHandle, any data1, any data2=0);
native bool SteamWorks_SetHTTPRequestNetworkActivityTimeout(Handle hHandle, int timeout);
native bool SteamWorks_SetHTTPRequestHeaderValue(Handle hHandle, const char[] sName, const char[] sValue);
native bool SteamWorks_SetHTTPRequestGetOrPostParameter(Handle hHandle, const char[] sName, const char[] sValue);
native bool SteamWorks_SetHTTPRequestUserAgentInfo(Handle hHandle, const char[] sUserAgentInfo);
native bool SteamWorks_SetHTTPRequestRequiresVerifiedCertificate(Handle hHandle, bool bRequireVerifiedCertificate);
native bool SteamWorks_SetHTTPRequestAbsoluteTimeoutMS(Handle hHandle, int unMilliseconds);


typeset SteamWorksHTTPRequestCompleted
{
	function void (Handle hRequest, bool bFailure, bool bRequestSuccessful, EHTTPStatusCode eStatusCode);
	function void (Handle hRequest, bool bFailure, bool bRequestSuccessful, EHTTPStatusCode eStatusCode, any data1);
	function void (Handle hRequest, bool bFailure, bool bRequestSuccessful, EHTTPStatusCode eStatusCode, any data1, any data2);
};

typeset SteamWorksHTTPHeadersReceived
{
	function void (Handle hRequest, bool bFailure);
	function void (Handle hRequest, bool bFailure, any data1);
	function void (Handle hRequest, bool bFailure, any data1, any data2);
};

typeset SteamWorksHTTPDataReceived
{
	function void (Handle hRequest, bool bFailure, int offset, int bytesreceived);
	function void (Handle hRequest, bool bFailure, int offset, int bytesreceived, any data1);
	function void (Handle hRequest, bool bFailure, int offset, int bytesreceived, any data1, any data2);
};

native bool SteamWorks_SetHTTPCallbacks(Handle hHandle, SteamWorksHTTPRequestCompleted fCompleted = INVALID_FUNCTION, SteamWorksHTTPHeadersReceived fHeaders = INVALID_FUNCTION, SteamWorksHTTPDataReceived fData = INVALID_FUNCTION, Handle hCalling = null);
native bool SteamWorks_SendHTTPRequest(Handle hRequest);
native bool SteamWorks_SendHTTPRequestAndStreamResponse(Handle hRequest);
native bool SteamWorks_DeferHTTPRequest(Handle hRequest);
native bool SteamWorks_PrioritizeHTTPRequest(Handle hRequest);
native bool SteamWorks_GetHTTPResponseHeaderSize(Handle hRequest, const char[] sHeader, int &size);
native bool SteamWorks_GetHTTPResponseHeaderValue(Handle hRequest, const char[] sHeader, char[] sValue, int size);
native bool SteamWorks_GetHTTPResponseBodySize(Handle hRequest, int &size);
native bool SteamWorks_GetHTTPResponseBodyData(Handle hRequest, char[] sBody, int length);
native bool SteamWorks_GetHTTPStreamingResponseBodyData(Handle hRequest, int cOffset, char[] sBody, int length);
native bool SteamWorks_GetHTTPDownloadProgressPct(Handle hRequest, float &percent);
native bool SteamWorks_GetHTTPRequestWasTimedOut(Handle hRequest, bool &bWasTimedOut);
native bool SteamWorks_SetHTTPRequestRawPostBody(Handle hRequest, const char[] sContentType, const char[] sBody, int bodylen);

typeset SteamWorksHTTPBodyCallback
{
	function void (const char[] sData);
	function void (const char[] sData, any value);
	function void (const char[] data, any value, int datalen);
};

native bool SteamWorks_GetHTTPResponseBodyCallback(Handle hRequest, SteamWorksHTTPBodyCallback fCallback, any data = 0, Handle hPlugin = null);
native bool SteamWorks_WriteHTTPResponseBodyToFile(Handle hRequest, const char[] sFileName);

forward void SW_OnValidateClient(int ownerauthid, int authid);
forward void SteamWorks_OnValidateClient(int ownerauthid, int authid);
forward void SteamWorks_SteamServersConnected();
forward void SteamWorks_SteamServersConnectFailure(EResult result);
forward void SteamWorks_SteamServersDisconnected(EResult result);

forward Action SteamWorks_RestartRequested();
forward void SteamWorks_TokenRequested(char[] sToken, int maxlen);

forward void SteamWorks_OnClientGroupStatus(int authid, int groupid, bool isMember, bool isOfficer);

forward EGCResults SteamWorks_GCSendMessage(int unMsgType, const char[] pubData, int cubData);
forward void SteamWorks_GCMsgAvailable(int cubData);
forward EGCResults SteamWorks_GCRetrieveMessage(int punMsgType, const char[] pubDest, int cubDest, int pcubMsgSize);

native EGCResults SteamWorks_SendMessageToGC(int unMsgType, const char[] pubData, int cubData);