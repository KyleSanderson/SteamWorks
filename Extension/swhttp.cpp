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

#include "swhttp.h"

static ISteamHTTP *GetHTTPPointer()
{
	return g_SteamWorks.pSWGameServer->GetHTTP();
}

SteamWorksHTTP::SteamWorksHTTP()
{
	this->typeHTTP = handlesys->CreateType("HTTPHandle", this, 0, NULL, NULL, myself->GetIdentity(), NULL);
}

SteamWorksHTTP::~SteamWorksHTTP()
{
	handlesys->RemoveType(this->typeHTTP, myself->GetIdentity());
}

HandleType_t SteamWorksHTTP::GetHTTPHandle(void)
{
	return this->typeHTTP;
}

static void DelayedDeleteSteamWorksHTTPRequest(void *object)
{
	SteamWorksHTTPRequest *pRequest = reinterpret_cast<SteamWorksHTTPRequest *>(object);
	delete pRequest;
}

void SteamWorksHTTP::OnHandleDestroy(HandleType_t type, void *object)
{
	if (type == this->typeHTTP) /* Heaven forbid we ever offer another handle. */
	{
		smutils->AddFrameAction(DelayedDeleteSteamWorksHTTPRequest, object);
	}
}

bool SteamWorksHTTP::GetHandleApproxSize(HandleType_t type, void *object, unsigned int *pSize)
{
	if (type == this->typeHTTP && pSize) /* Heaven forbid we ever offer another handle. */
	{
		*pSize = sizeof(SteamWorksHTTPRequest);
		return true;
	}

	return false;
}
