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

#include "swhttprequest.h"

#include <cstdio>

static ISteamHTTP *GetHTTPPointer(void)
{
	return g_SteamWorks.pSWGameServer->GetHTTP();
}

static HandleType_t GetSteamHTTPHandle(void)
{
	return g_SteamWorks.pSWHTTP->GetHTTPHandle();
}

static SteamWorksHTTPRequest *GetRequestPointer(ISteamHTTP *&pHTTP, IPluginContext *pContext, cell_t Handle)
{
	pHTTP = GetHTTPPointer();
	if (pHTTP == NULL)
	{
		return NULL;
	}

	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	SteamWorksHTTPRequest *pRequest;
	if ((err = handlesys->ReadHandle(Handle, GetSteamHTTPHandle(), &sec, (void **)&pRequest))
		!= HandleError_None)
	{
		pContext->ThrowNativeError("Invalid Handle %x (error: %d)", Handle, err);
		return NULL;
	}

	return pRequest;
}

SteamWorksHTTPRequest::SteamWorksHTTPRequest() : request(INVALID_HTTPREQUEST_HANDLE), handle(BAD_HANDLE), pCompletedForward(NULL), pHeadersReceivedForward(NULL), pDataReceivedForward(NULL),
	m_CallbackHTTPRequestHeadersReceived(), m_CallbackHTTPRequestDataReceived()
{
};

SteamWorksHTTPRequest::~SteamWorksHTTPRequest()
{
	this->m_CallbackHTTPRequestHeadersReceived.Unregister();
	this->m_CallbackHTTPRequestDataReceived.Unregister();

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

void SteamWorksHTTPRequest::OnHTTPHeadersReceived(HTTPRequestHeadersReceived_t* pRequest)
{
	if (this->pHeadersReceivedForward == NULL || this->pHeadersReceivedForward->GetFunctionCount() == 0 || pRequest->m_hRequest != this->request)
	{
		return;
	}

	this->pHeadersReceivedForward->PushCell(this->handle);
	this->pHeadersReceivedForward->PushCell(false);
	this->pHeadersReceivedForward->PushCell(pRequest->m_ulContextValue >> 32);
	this->pHeadersReceivedForward->PushCell((pRequest->m_ulContextValue & 0x00000000FFFFFFFF));
	this->pHeadersReceivedForward->Execute(NULL);
}

void SteamWorksHTTPRequest::OnHTTPDataReceived(HTTPRequestDataReceived_t* pRequest)
{
	if (this->pDataReceivedForward == NULL || this->pDataReceivedForward->GetFunctionCount() == 0 || pRequest->m_hRequest != this->request)
	{
		return;
	}

	this->pDataReceivedForward->PushCell(this->handle);
	this->pDataReceivedForward->PushCell(false);
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
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	return pHTTP->SetHTTPRequestContextValue(pRequest->request, (static_cast<uint64_t>(params[2]) << 32 | static_cast<uint32_t>(params[3]))) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestNetworkActivityTimeout(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	return pHTTP->SetHTTPRequestNetworkActivityTimeout(pRequest->request, params[2]) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestHeaderValue(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pName, *pValue;
	pContext->LocalToString(params[2], &pName);
	pContext->LocalToString(params[3], &pValue);
	return pHTTP->SetHTTPRequestHeaderValue(pRequest->request, pName, pValue) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestGetOrPostParameter(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pName, *pValue;
	pContext->LocalToString(params[2], &pName);
	pContext->LocalToString(params[3], &pValue);
	return pHTTP->SetHTTPRequestGetOrPostParameter(pRequest->request, pName, pValue) ? 1 : 0;
}

static cell_t sm_SetCallbacks(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	IPlugin *pPlugin;
	if (params[5] == BAD_HANDLE)
	{
		pPlugin = plsys->FindPluginByContext(pContext->GetContext());
	} else {
		HandleError err;
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

static void SetCallbacks(SteamAPICall_t &hCall, SteamWorksHTTPRequest *pRequest)
{
	if (pRequest->pCompletedForward != NULL)
	{
		pRequest->CompletedCallResult.SetGameserverFlag();
		pRequest->CompletedCallResult.Set(hCall, pRequest, &SteamWorksHTTPRequest::OnHTTPRequestCompleted);
	}

	if (pRequest->pHeadersReceivedForward != NULL)
	{
		pRequest->m_CallbackHTTPRequestHeadersReceived.Register(pRequest, &SteamWorksHTTPRequest::OnHTTPHeadersReceived);
	}

	if (pRequest->pDataReceivedForward != NULL)
	{
		pRequest->m_CallbackHTTPRequestDataReceived.Register(pRequest, &SteamWorksHTTPRequest::OnHTTPDataReceived);
	}
}

static cell_t sm_SendHTTPRequestAndStreamResponse(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	SteamAPICall_t hCall;
	cell_t result = pHTTP->SendHTTPRequestAndStreamResponse(pRequest->request, &hCall) ? 1 : 0;

	SetCallbacks(hCall, pRequest);
	return result;
}

static cell_t sm_SendHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	SteamAPICall_t hCall;
	cell_t result = pHTTP->SendHTTPRequest(pRequest->request, &hCall) ? 1 : 0;

	SetCallbacks(hCall, pRequest);
	return result;
}

static cell_t sm_DeferHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	return pHTTP->DeferHTTPRequest(pRequest->request) ? 1 : 0;
}

static cell_t sm_PrioritizeHTTPRequest(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	return pHTTP->PrioritizeHTTPRequest(pRequest->request) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseHeaderSize(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);
	
	cell_t *pSize;
	pContext->LocalToPhysAddr(params[3], &pSize);
	return pHTTP->GetHTTPResponseHeaderSize(pRequest->request, pName, reinterpret_cast<uint32_t *>(pSize)) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseHeaderValue(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);
	
	char *pBuffer;
	pContext->LocalToString(params[3], &pBuffer);
	return pHTTP->GetHTTPResponseHeaderValue(pRequest->request, pName, reinterpret_cast<uint8_t *>(pBuffer), params[4]) ? 1 : 0; /* uint8? wtf? */
}

static cell_t sm_GetHTTPResponseBodySize(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	cell_t *pSize;
	pContext->LocalToPhysAddr(params[2], &pSize);
	return pHTTP->GetHTTPResponseBodySize(pRequest->request, reinterpret_cast<uint32_t *>(pSize)) ? 1 : 0;
}

static cell_t sm_GetHTTPResponseBodyData(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pBuffer;
	pContext->LocalToString(params[2], &pBuffer);
	return pHTTP->GetHTTPResponseBodyData(pRequest->request, reinterpret_cast<uint8_t *>(pBuffer), params[3]) ? 1 : 0;
}

static cell_t sm_GetHTTPDownloadProgressPct(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
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
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pName, *pBuffer;
	pContext->LocalToString(params[2], &pName);
	pContext->LocalToString(params[3], &pBuffer);

	return pHTTP->SetHTTPRequestRawPostBody(pRequest->request, pName, reinterpret_cast<uint8_t *>(pBuffer), params[4]) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestRawPostBodyFromFile(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pContentType, *pFilePath;
	pContext->LocalToString(params[2], &pContentType);
	pContext->LocalToString(params[3], &pFilePath);

	char szFinalPath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, szFinalPath, sizeof(szFinalPath), "%s", pFilePath);

	FILE *pInputFile = fopen(szFinalPath, "rb");
	if (!pInputFile)
	{
		return pContext->ThrowNativeError("Unable to open %s for reading. errno: %d", szFinalPath, errno);
	}

	fseek(pInputFile, 0, SEEK_END);
	uint32_t size = ftell(pInputFile);
	fseek(pInputFile, 0, SEEK_SET);

	if (size <= 0)
	{
		fclose(pInputFile);
		return 0;
	}

	char *pBuffer = new char[size + 1];
	uint32_t itemsRead = fread(pBuffer, sizeof(char), size, pInputFile);
	fclose(pInputFile);

	if (itemsRead != size)
	{
		delete [] pBuffer;
		return 0;
	}

	cell_t result = pHTTP->SetHTTPRequestRawPostBody(pRequest->request, pContentType, reinterpret_cast<uint8_t *>(pBuffer), size) ? 1 : 0;

	delete [] pBuffer;
	return result;
}

static cell_t sm_GetHTTPResponseBodyCallback(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	IPlugin *pPlugin;
	if (params[4] == BAD_HANDLE)
	{
		pPlugin = plsys->FindPluginByContext(pContext->GetContext());
	} else {
		HandleError err;
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

	char *pBuffer = new char[size+1];

	if (pHTTP->GetHTTPResponseBodyData(pRequest->request, reinterpret_cast<uint8_t *>(pBuffer), size) == false)
	{
		delete [] pBuffer;
		return 0;
	}

	pBuffer[size] = '\0'; /* Incase users do something bad; we want to protect userspace; kind of. */
	pFunction->PushStringEx(pBuffer, size + 1, SM_PARAM_STRING_BINARY | SM_PARAM_STRING_COPY, 0);
	pFunction->PushCell(params[3]);
	pFunction->PushCell(size);
	pFunction->Execute(NULL);

	delete [] pBuffer;
	return 1;
}

static cell_t sm_WriteHTTPResponseBodyToFile(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	uint32_t size;
	if (pHTTP->GetHTTPResponseBodySize(pRequest->request, &size) == false)
	{
		return 0;
	}

	char *pBuffer = new char[size];

	if (pHTTP->GetHTTPResponseBodyData(pRequest->request, reinterpret_cast<uint8_t *>(pBuffer), size) == false)
	{
		delete [] pBuffer;
		return 0;
	}

	char *pFilePath;
	pContext->LocalToString(params[2], &pFilePath);

	char szFinalPath[PLATFORM_MAX_PATH];
	g_pSM->BuildPath(Path_Game, szFinalPath, sizeof(szFinalPath), "%s", pFilePath);

	FILE *pOutputFile = fopen(szFinalPath, "wb");

	if (!pOutputFile)
	{
		delete [] pBuffer;
		return pContext->ThrowNativeError("Unable to open %s for writing. errno: %d", szFinalPath, errno);
	}

	fwrite(pBuffer, sizeof(char), size, pOutputFile);

	delete [] pBuffer;
	fclose(pOutputFile);

	return 1;
}

static cell_t sm_GetHTTPStreamingResponseBodyData(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pBuffer;
	pContext->LocalToString(params[3], &pBuffer);
	return pHTTP->GetHTTPStreamingResponseBodyData(pRequest->request, params[2], reinterpret_cast<uint8_t *>(pBuffer), params[4]) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestUserAgentInfo(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	char *pName;
	pContext->LocalToString(params[2], &pName);

	return pHTTP->SetHTTPRequestUserAgentInfo(pRequest->request, pName) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestRequiresVerifiedCertificate(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	return pHTTP->SetHTTPRequestRequiresVerifiedCertificate(pRequest->request, !!params[2]) ? 1 : 0;
}

static cell_t sm_SetHTTPRequestAbsoluteTimeoutMS(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	return pHTTP->SetHTTPRequestAbsoluteTimeoutMS(pRequest->request, params[2]) ? 1 : 0;
}

static cell_t sm_GetHTTPRequestWasTimedOut(IPluginContext *pContext, const cell_t *params)
{
	ISteamHTTP *pHTTP;
	SteamWorksHTTPRequest *pRequest = GetRequestPointer(pHTTP, pContext, params[1]);
	if (pRequest == NULL)
	{
		return 0;
	}

	cell_t *pData;
	pContext->LocalToPhysAddr(params[2], &pData);

	return pHTTP->GetHTTPRequestWasTimedOut(pRequest->request, reinterpret_cast<bool *>(pData)) ? 1 : 0;
}

static sp_nativeinfo_t httpnatives[] = {
	{"SteamWorks_CreateHTTPRequest",				sm_CreateHTTPRequest},
	{"SteamWorks_SetHTTPRequestContextValue",				sm_SetHTTPRequestContextValue},
	{"SteamWorks_SetHTTPRequestNetworkActivityTimeout",				sm_SetHTTPRequestNetworkActivityTimeout},
	{"SteamWorks_SetHTTPRequestHeaderValue",				sm_SetHTTPRequestHeaderValue},
	{"SteamWorks_SetHTTPRequestGetOrPostParameter",				sm_SetHTTPRequestGetOrPostParameter},
	{"SteamWorks_SetHTTPRequestUserAgentInfo",					sm_SetHTTPRequestUserAgentInfo},
	{"SteamWorks_SetHTTPRequestRequiresVerifiedCertificate",				sm_SetHTTPRequestRequiresVerifiedCertificate},
	{"SteamWorks_SetHTTPRequestAbsoluteTimeoutMS",						sm_SetHTTPRequestAbsoluteTimeoutMS},
	{"SteamWorks_SetHTTPCallbacks",				sm_SetCallbacks},
	{"SteamWorks_SendHTTPRequest",				sm_SendHTTPRequest},
	{"SteamWorks_DeferHTTPRequest",				sm_DeferHTTPRequest},
	{"SteamWorks_PrioritizeHTTPRequest",				sm_PrioritizeHTTPRequest},
	{"SteamWorks_GetHTTPResponseHeaderSize",				sm_GetHTTPResponseHeaderSize},
	{"SteamWorks_GetHTTPResponseHeaderValue",				sm_GetHTTPResponseHeaderValue},
	{"SteamWorks_GetHTTPResponseBodySize",				sm_GetHTTPResponseBodySize},
	{"SteamWorks_GetHTTPResponseBodyData",				sm_GetHTTPResponseBodyData},
	{"SteamWorks_GetHTTPDownloadProgressPct",				sm_GetHTTPDownloadProgressPct},
	{"SteamWorks_GetHTTPRequestWasTimedOut",				sm_GetHTTPRequestWasTimedOut},
	{"SteamWorks_SetHTTPRequestRawPostBody",				sm_SetHTTPRequestRawPostBody},
	{"SteamWorks_SetHTTPRequestRawPostBodyFromFile",		sm_SetHTTPRequestRawPostBodyFromFile},
	{"SteamWorks_GetHTTPResponseBodyCallback",				sm_GetHTTPResponseBodyCallback},
	{"SteamWorks_WriteHTTPResponseBodyToFile",				sm_WriteHTTPResponseBodyToFile},
	{"SteamWorks_SendHTTPRequestAndStreamResponse",			sm_SendHTTPRequestAndStreamResponse},
	{"SteamWorks_GetHTTPStreamingResponseBodyData",			sm_GetHTTPStreamingResponseBodyData},
	{NULL,											NULL}
};

SteamWorksHTTPNatives::SteamWorksHTTPNatives()
{
	sharesys->AddNatives(myself, httpnatives);
}

SteamWorksHTTPNatives::~SteamWorksHTTPNatives()
{
}
