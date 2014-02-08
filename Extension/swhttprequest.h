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

#pragma once
#include "isteamgameserver.h"
#include "steam_gameserver.h"
#include "smsdk_ext.h"

class SteamWorksHTTPRequest
{
	public:
		SteamWorksHTTPRequest();
		~SteamWorksHTTPRequest();

	public:
		HTTPRequestHandle request;
		Handle_t handle;

	public:
		void OnHTTPRequestCompleted(HTTPRequestCompleted_t *pRequest, bool bFailed);
		void OnHTTPHeadersReceived(HTTPRequestHeadersReceived_t *pRequest, bool bFailed);
		void OnHTTPDataReceived(HTTPRequestDataReceived_t *pRequest, bool bFailed);

	public:
		CCallResult<SteamWorksHTTPRequest, HTTPRequestCompleted_t> CompletedCallResult;
		CCallResult<SteamWorksHTTPRequest, HTTPRequestHeadersReceived_t> HeadersCallResult;
		CCallResult<SteamWorksHTTPRequest, HTTPRequestDataReceived_t> DataCallResult;

	public:
		IChangeableForward *pCompletedForward;
		IChangeableForward *pHeadersReceivedForward;
		IChangeableForward *pDataReceivedForward;
};

class SteamWorksHTTPNatives
{
	public:
		SteamWorksHTTPNatives();
		~SteamWorksHTTPNatives();
};

#include "extension.h"