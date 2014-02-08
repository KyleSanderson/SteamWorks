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

#include "swhttprequest.h"

static ISteamHTTP *GetHTTPPointer(void)
{
	return g_SteamWorks.pSWGameServer->GetHTTP();
}

static HandleType_t GetSteamHTTPHandle(void)
{
	return g_SteamWorks.pSWHTTP->GetHTTPHandle();
}

SteamWorksHTTPRequest::SteamWorksHTTPRequest()
{
	this->request = INVALID_HTTPREQUEST_HANDLE;
	this->handle = BAD_HANDLE;
	this->pCompletedForward = NULL;
	this->pHeadersReceivedForward = NULL;
	this->pDataReceivedForward = NULL;
};

SteamWorksHTTPRequest::~SteamWorksHTTPRequest()
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP != NULL)
	{
		pHTTP->ReleaseHTTPRequest(this->request);
		this->request = INVALID_HTTPREQUEST_HANDLE;
	}

	forwards->ReleaseForward(this->pCompletedForward);
	this->pCompletedForward = NULL;

	forwards->ReleaseForward(this->pHeadersReceivedForward);
	this->pHeadersReceivedForward = NULL;

	forwards->ReleaseForward(this->pDataReceivedForward);
	this->pDataReceivedForward = NULL;
}

/* We pay the Iron Price. */
void SteamWorksHTTPRequest::OnHTTPRequestCompleted(HTTPRequestCompleted_t *pRequest, bool bFailed)
{
	if (this->pCompletedForward == NULL || this->pCompletedForward->GetFunctionCount() == 0)
	{
		return;
	}

	this->pCompletedForward->PushCell(this->handle);
	this->pCompletedForward->PushCell(bFailed);
	this->pCompletedForward->PushCell(pRequest->m_bRequestSuccessful);
	this->pCompletedForward->PushCell(pRequest->m_eStatusCode);
	this->pCompletedForward->PushCell(pRequest->m_ulContextValue >> 32);
	this->pCompletedForward->PushCell((pRequest->m_ulContextValue & 0x00000000FFFFFFFF));
	this->pCompletedForward->Execute(NULL);
}

void SteamWorksHTTPRequest::OnHTTPHeadersReceived(HTTPRequestHeadersReceived_t *pRequest, bool bFailed)
{
	if (this->pHeadersReceivedForward == NULL || this->pHeadersReceivedForward->GetFunctionCount() == 0)
	{
		return;
	}

	this->pHeadersReceivedForward->PushCell(this->handle);
	this->pHeadersReceivedForward->PushCell(bFailed);
	this->pHeadersReceivedForward->PushCell(pRequest->m_ulContextValue >> 32);
	this->pHeadersReceivedForward->PushCell((pRequest->m_ulContextValue & 0x00000000FFFFFFFF));
	this->pHeadersReceivedForward->Execute(NULL);
}

void SteamWorksHTTPRequest::OnHTTPDataReceived(HTTPRequestDataReceived_t *pRequest, bool bFailed)
{
	if (this->pDataReceivedForward == NULL || this->pDataReceivedForward->GetFunctionCount() == 0)
	{
		return;
	}

	this->pDataReceivedForward->PushCell(this->handle);
	this->pDataReceivedForward->PushCell(bFailed);
	this->pDataReceivedForward->PushCell(pRequest->m_cOffset);
	this->pDataReceivedForward->PushCell(pRequest->m_cBytesReceived);
	this->pDataReceivedForward->PushCell(pRequest->m_ulContextValue >> 32);
	this->pDataReceivedForward->PushCell((pRequest->m_ulContextValue & 0x00000000FFFFFFFF));
	this->pDataReceivedForward->Execute(NULL);
}

static cell_t sm_CreateHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return BAD_HANDLE;
	}

	char *pURL;
	pContext->LocalToString(params[2], &pURL);

	HTTPRequestHandle request = pHTTP->CreateHTTPRequest(static_cast<EHTTPMethod>(params[1]), pURL);
	if (request == INVALID_HTTPREQUEST_HANDLE)
	{
		return BAD_HANDLE;
	}

	SteamWorksHTTPRequest *pRequest = new SteamWorksHTTPRequest;
	Handle_t handle = handlesys->CreateHandle(GetSteamHTTPHandle(), pRequest, pContext->GetIdentity(), myself->GetIdentity(), NULL);
	if (handle == BAD_HANDLE)
	{
		pHTTP->ReleaseHTTPRequest(request);
		delete pRequest;
		return BAD_HANDLE;
	}

	pRequest->request = request;
	pRequest->handle = handle;
	
	return handle;
}

static cell_t sm_SetHTTPRequestContextValue(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	
	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	return pHTTP->SetHTTPRequestContextValue(pRequest->request, (static_cast<uint64_t>(params[2]) << 32 | params[3])) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestNetworkActivityTimeout(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	return pHTTP->SetHTTPRequestNetworkActivityTimeout(pRequest->request, params[2]) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestHeaderValue(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	char *pName, *pValue;
	pContext->LocalToString(params[2], &pName);
	pContext->LocalToString(params[3], &pValue);
	return pHTTP->SetHTTPRequestHeaderValue(pRequest->request, pName, pValue) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestGetOrPostParameter(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	char *pName, *pValue;
	pContext->LocalToString(params[2], &pName);
	pContext->LocalToString(params[3], &pValue);
	return pHTTP->SetHTTPRequestGetOrPostParameter(pRequest->request, pName, pValue) ? 1 : 0;
}

static cell_t sm_SetCallbacks(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	IPlugin *pPlugin;
	if (params[5] == BAD_HANDLE)
	{
		pPlugin = plsys->FindPluginByContext(pContext->GetContext());
	} else {
		pPlugin = plsys->PluginFromHandle(params[5], &err);

		if (!pPlugin)
		{
			return pContext->ThrowNativeError("Plugin handle %x is invalid (error %d)", params[5], err);
		}
	}

	if (params[2] > 0)
	{
		if (pRequest->pCompletedForward == NULL)
		{
			pRequest->pCompletedForward = forwards->CreateForwardEx(NULL, ET_Ignore, 6, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
		}

		IPluginFunction *pFunction = pPlugin->GetBaseContext()->GetFunctionById(params[2]);

		if (!pFunction)
		{
			return pContext->ThrowNativeError("Invalid function id (%X)", params[2]);
		}

		pRequest->pCompletedForward->AddFunction(pFunction);
	}
	
	if (params[3] > 0)
	{
		if (pRequest->pHeadersReceivedForward == NULL)
		{
			pRequest->pHeadersReceivedForward = forwards->CreateForwardEx(NULL, ET_Ignore, 4, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
		}

		IPluginFunction *pFunction = pPlugin->GetBaseContext()->GetFunctionById(params[3]);

		if (!pFunction)
		{
			return pContext->ThrowNativeError("Invalid function id (%X)", params[3]);
		}

		pRequest->pHeadersReceivedForward->AddFunction(pFunction);
	}

	if (params[4] > 0)
	{
		if (pRequest->pDataReceivedForward == NULL)
		{
			pRequest->pDataReceivedForward = forwards->CreateForwardEx(NULL, ET_Ignore, 6, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
		}

		IPluginFunction *pFunction = pPlugin->GetBaseContext()->GetFunctionById(params[4]);

		if (!pFunction)
		{
			return pContext->ThrowNativeError("Invalid function id (%X)", params[4]);
		}

		pRequest->pDataReceivedForward->AddFunction(pFunction);
	}

	return 1;
}

static cell_t sm_SendHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	SteamAPICall_t hCall;
	cell_t result = pHTTP->SendHTTPRequest(pRequest->request, &hCall) ? 1 : 0;

	if (pRequest->pCompletedForward != NULL)
	{
		pRequest->CompletedCallResult.SetGameserverFlag();
		pRequest->CompletedCallResult.Set(hCall, pRequest, &SteamWorksHTTPRequest::OnHTTPRequestCompleted);
	}

	if (pRequest->pHeadersReceivedForward != NULL)
	{
		pRequest->HeadersCallResult.SetGameserverFlag();
		pRequest->HeadersCallResult.Set(hCall, pRequest, &SteamWorksHTTPRequest::OnHTTPHeadersReceived);
	}

	if (pRequest->pDataReceivedForward != NULL)
	{
		pRequest->DataCallResult.SetGameserverFlag();
		pRequest->DataCallResult.Set(hCall, pRequest, &SteamWorksHTTPRequest::OnHTTPDataReceived);
	}

	return result;
}

static cell_t sm_DeferHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	return pHTTP->DeferHTTPRequest(pRequest->request) ? 1 : 0;
}

static cell_t sm_PrioritizeHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	return pHTTP->PrioritizeHTTPRequest(pRequest->request) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseHeaderSize(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);
	
	cell_t *pSize;
	pContext->LocalToPhysAddr(params[3], &pSize);
	return pHTTP->GetHTTPResponseHeaderSize(pRequest->request, pName, reinterpret_cast<uint32_t *>(pSize)) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseHeaderValue(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);
	
	char *pBuffer;
	pContext->LocalToString(params[3], &pBuffer);
	return pHTTP->GetHTTPResponseHeaderValue(pRequest->request, pName, reinterpret_cast<uint8_t *>(pBuffer), params[4]) ? 1 : 0; /* uint8? wtf? */
}

static cell_t sm_GetHTTPResponseBodySize(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	cell_t *pSize;
	pContext->LocalToPhysAddr(params[2], &pSize);
	return pHTTP->GetHTTPResponseBodySize(pRequest->request, reinterpret_cast<uint32_t *>(pSize)) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseBodyData(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	char *pBuffer;
	pContext->LocalToString(params[2], &pBuffer);
	return pHTTP->GetHTTPResponseBodyData(pRequest->request, reinterpret_cast<uint8_t *>(pBuffer), params[3]) ? 1 : 0;
}

static cell_t sm_GetHTTPDownloadProgressPct(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	float percent;
	cell_t result = pHTTP->GetHTTPDownloadProgressPct(pRequest->request, &percent) ? 1 : 0;
	
	cell_t *pData;
	pContext->LocalToPhysAddr(params[2], &pData);
	*pData = sp_ftoc(percent);
	return result;
}

static cell_t sm_SetHTTPRequestRawPostBody(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	char *pName, *pBuffer;
	pContext->LocalToString(params[2], &pName);
	pContext->LocalToString(params[3], &pBuffer);

	return pHTTP->SetHTTPRequestRawPostBody(pRequest->request, pName, reinterpret_cast<uint8_t *>(pBuffer), params[4]) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseBodyCallback(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return 0;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(params[1], GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid Handle %x (error: %d)", params[1], err);
	}

	IPlugin *pPlugin;
	if (params[4] == BAD_HANDLE)
	{
		pPlugin = plsys->FindPluginByContext(pContext->GetContext());
	} else {
		pPlugin = plsys->PluginFromHandle(params[4], &err);

		if (!pPlugin)
		{
			return pContext->ThrowNativeError("Plugin handle %x is invalid (error %d)", params[4], err);
		}
	}

	IPluginFunction *pFunction = pPlugin->GetBaseContext()->GetFunctionById(params[2]);
	if (!pFunction)
	{
		return pContext->ThrowNativeError("Invalid function id (%X)", params[2]);
	}

	uint32_t size;
	if (pHTTP->GetHTTPResponseBodySize(pRequest->request, &size) == false)
	{
		return 0;
	}

	size_t celllen = ((size / sizeof(cell_t)) + 1);
	cell_t *pBuffer = new cell_t[celllen+1];

	if (pHTTP->GetHTTPResponseBodyData(pRequest->request, reinterpret_cast<uint8_t *>(pBuffer), size) == false)
	{
		return 0;
	}

	pBuffer[celllen] = '\0'; /* Incase users do something bad; we want to protect userspace; kind of. */
	pFunction->PushArray(pBuffer, celllen); /* Strings do a copy... it's really bad :( */
	pFunction->PushCell(params[3]);
	pFunction->PushCell(celllen);
	pFunction->Execute(NULL);

	delete pBuffer;
	return 1;
}

static sp_nativeinfo_t httpnatives[] = {
	{"SteamWorks_CreateHTTPRequest",				sm_CreateHTTPRequest},
	{"SteamWorks_SetHTTPRequestContextValue",				sm_SetHTTPRequestContextValue},
	{"SteamWorks_SetHTTPRequestNetworkActivityTimeout",				sm_SetHTTPRequestNetworkActivityTimeout},
	{"SteamWorks_SetHTTPRequestHeaderValue",				sm_SetHTTPRequestHeaderValue},
	{"SteamWorks_SetHTTPRequestGetOrPostParameter",				sm_SetHTTPRequestGetOrPostParameter},
	{"SteamWorks_SetHTTPCallbacks",				sm_SetCallbacks},
	{"SteamWorks_SendHTTPRequest",				sm_SendHTTPRequest},
	{"SteamWorks_DeferHTTPRequest",				sm_DeferHTTPRequest},
	{"SteamWorks_PrioritizeHTTPRequest",				sm_PrioritizeHTTPRequest},
	{"SteamWorks_GetHTTPResponseHeaderSize",				sm_GetHTTPResponseHeaderSize},
	{"SteamWorks_GetHTTPResponseHeaderValue",				sm_GetHTTPResponseHeaderValue},
	{"SteamWorks_GetHTTPResponseBodySize",				sm_GetHTTPResponseBodySize},
	{"SteamWorks_GetHTTPResponseBodyData",				sm_GetHTTPResponseBodyData},
	{"SteamWorks_GetHTTPDownloadProgressPct",				sm_GetHTTPDownloadProgressPct},
	{"SteamWorks_SetHTTPRequestRawPostBody",				sm_SetHTTPRequestRawPostBody},
	{"SteamWorks_GetHTTPResponseBodyCallback",				sm_GetHTTPResponseBodyCallback},
	{NULL,											NULL}
};

SteamWorksHTTPNatives::SteamWorksHTTPNatives()
{
	sharesys->AddNatives(myself, httpnatives);
}

SteamWorksHTTPNatives::~SteamWorksHTTPNatives()
{
}
