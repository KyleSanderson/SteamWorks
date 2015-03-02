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

#include "swforwards.h"

SteamWorksForwards::SteamWorksForwards() :
		m_CallbackGSClientApprove(this, &SteamWorksForwards::OnGSClientApprove),
		m_CallbackValidateTicket(this, &SteamWorksForwards::OnValidateTicket),
		m_CallbackSteamConnected(this, &SteamWorksForwards::OnSteamServersConnected),
		m_CallbackSteamConnectFailure(this, &SteamWorksForwards::OnSteamServersConnectFailure),
		m_CallbackSteamDisconnected(this, &SteamWorksForwards::OnSteamServersDisconnected),
		m_CallbackGroupStatus(this, &SteamWorksForwards::OnGroupStatusResult)
{
	this->pFOVC_Old = forwards->CreateForward("SW_OnValidateClient", ET_Ignore, 2, NULL, Param_Cell, Param_Cell);
	this->pFOVC = forwards->CreateForward("SteamWorks_OnValidateClient", ET_Ignore, 2, NULL, Param_Cell, Param_Cell);
	this->pFOSSC = forwards->CreateForward("SteamWorks_SteamServersConnected", ET_Ignore, 0, NULL);
	this->pFOSSCF = forwards->CreateForward("SteamWorks_SteamServersConnectFailure", ET_Ignore, 1, NULL, Param_Cell);
	this->pFOSSD = forwards->CreateForward("SteamWorks_SteamServersDisconnected", ET_Ignore, 1, NULL, Param_Cell);
	this->pFOCGS = forwards->CreateForward("SteamWorks_OnClientGroupStatus", ET_Ignore, 4, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
}

SteamWorksForwards::~SteamWorksForwards()
{
	forwards->ReleaseForward(this->pFOVC_Old);
	forwards->ReleaseForward(this->pFOVC);
	forwards->ReleaseForward(this->pFOSSC);
	forwards->ReleaseForward(this->pFOSSCF);
	forwards->ReleaseForward(this->pFOSSD);
	forwards->ReleaseForward(this->pFOCGS);
}

void SteamWorksForwards::NotifyPawnValidateClient(Account_t parent, Account_t child)
{
	if (this->pFOVC_Old->GetFunctionCount() != 0)
	{
		this->pFOVC_Old->PushCell(parent);
		this->pFOVC_Old->PushCell(child);
		this->pFOVC_Old->Execute(NULL);
	}

	if (this->pFOVC->GetFunctionCount() != 0)
	{
		this->pFOVC->PushCell(parent);
		this->pFOVC->PushCell(child);
		this->pFOVC->Execute(NULL);
	}
}

void SteamWorksForwards::OnGSClientApprove(GSClientApprove_t *pApprove)
{
	this->NotifyPawnValidateClient(pApprove->m_OwnerSteamID.GetAccountID(), pApprove->m_SteamID.GetAccountID());
}

void SteamWorksForwards::OnValidateTicket(ValidateAuthTicketResponse_t *pTicket)
{
	this->NotifyPawnValidateClient(pTicket->m_OwnerSteamID.GetAccountID(), pTicket->m_SteamID.GetAccountID());
}

void SteamWorksForwards::OnSteamServersConnected(SteamServersConnected_t *pResponse)
{
	if (this->pFOSSC->GetFunctionCount() == 0)
	{
		return;
	}
	
	this->pFOSSC->Execute(NULL);
}

void SteamWorksForwards::OnSteamServersConnectFailure(SteamServerConnectFailure_t *pResponse)
{
	if (this->pFOSSCF->GetFunctionCount() == 0)
	{
		return;
	}
	
	this->pFOSSCF->PushCell(pResponse->m_eResult);
	this->pFOSSCF->Execute(NULL);
}

void SteamWorksForwards::OnSteamServersDisconnected(SteamServersDisconnected_t *pResponse)
{
	if (this->pFOSSD->GetFunctionCount() == 0)
	{
		return;
	}
	
	this->pFOSSD->PushCell(pResponse->m_eResult);
	this->pFOSSD->Execute(NULL);
}

void SteamWorksForwards::OnGroupStatusResult(GSClientGroupStatus_t *pResponse)
{
	if (this->pFOCGS->GetFunctionCount() == 0)
	{
		return;
	}

	this->pFOCGS->PushCell(pResponse->m_SteamIDUser.GetAccountID());
	this->pFOCGS->PushCell(pResponse->m_SteamIDGroup.GetAccountID());
	this->pFOCGS->PushCell(pResponse->m_bMember);
	this->pFOCGS->PushCell(pResponse->m_bOfficer);
	this->pFOCGS->Execute(NULL);
}