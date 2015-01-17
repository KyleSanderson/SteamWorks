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

#pragma once

#include "blob.h"
class GCTokenSection
{
public:
	GCTokenSection(CBlob &blob)
	{
		blob.Read(this->length);
		
		if (this->length != this->expectedlen)
		{
			this->bValid = false;
			return;
		}
		
		blob.Read(this->token);
		blob.Read(this->steamid);
		this->bValid = blob.Read(this->generation);
	};
public:
	bool IsValid(void)
	{
		return this->bValid;
	}

public:  /* No ideal packing, but it saves us from having to comment. */
	static const uint32 expectedlen = 20; /* Magic number for GC? */
	uint32 length;
	uint64 token;
	CSteamID steamid;
	time_t generation;
	bool bValid;
};

class SessionSection
{
public:
	SessionSection(CBlob &blob)
	{
		blob.Read(this->length);
		
		if (this->length != this->expectedlen)
		{
			this->bValid = false;
			return;
		}
		
		blob.Read(this->unk1);
		blob.Read(this->unk2);
		blob.Read(this->externalip);
		blob.Read(this->filler);
		blob.Read(this->timestamp);
		this->bValid = blob.Read(this->connectioncount);
	}
public:
	bool IsValid(void)
	{
		return this->bValid;
	}

public:
	static const uint32 expectedlen = 24; /* Magic number for Session? */
	uint32 length;
	uint32 unk1;
	uint32 unk2;
	uint32 externalip;
	uint32 filler;
	uint32 timestamp;
	uint32 connectioncount;
	bool bValid;
};

class DLCInfo
{
public:
	DLCInfo() : pSub(NULL)
	{
	};
	
	~DLCInfo()
	{
		delete [] this->pSub;
	};
public:
	void TakeBlob(CBlob &blob)
	{
		blob.Read(this->appid);
		blob.Read(this->subcount);
		
		this->pSub = new uint32[this->subcount];

		if (this->pSub == NULL)
		{
			return;
		}

		for (size_t iter = 0; iter < this->subcount; ++iter)
		{
			blob.Read(this->pSub[iter]);
		}
	}
public:
	uint32 appid;
	uint16 subcount;
	uint32 *pSub;
};

class OwnershipSection
{
public:
	OwnershipSection(CBlob &blob)
	{
		blob.Read(this->length);
		
		if (this->length == 0)
		{
			this->pLicense = NULL;
			this->pDLC = NULL;
			this->bValid = false;
			return;
		}
		
		blob.Read(this->length);
		blob.Read(this->version);
		blob.Read(this->steamid);
		blob.Read(this->appid);
		blob.Read(this->externalIP);
		blob.Read(this->internalIP);
		blob.Read(this->ownershipFlags);
		blob.Read(this->ticketGeneration);
		blob.Read(this->ticketExpiration);
		blob.Read(this->licenseCount);
		
		this->pLicense = new uint32[this->licenseCount];

		if (this->pLicense == NULL)
		{
			this->pDLC = NULL;
			this->bValid = false;
			return;
		}

		for (size_t iter = 0; iter < this->licenseCount; ++iter)
		{
			blob.Read(this->pLicense[iter]);
		}
		
		blob.Read(this->dlcCount);
		this->pDLC = new DLCInfo[this->dlcCount];
		if (this->pDLC == NULL)
		{
			this->bValid = false;
			return;
		}
		
		for (size_t iter = 0; iter < this->dlcCount; ++iter)
		{
			this->pDLC[iter].TakeBlob(blob);
		}
		
		blob.Read(this->reserved);
		bValid = blob.Read(this->signature, sizeof(this->signature));
	};
	
	~OwnershipSection()
	{
		delete [] this->pLicense;
		delete [] this->pDLC;
	}
public:
	bool IsValid(void)
	{
		return this->bValid;
	}

public:
	uint32 length;
	uint32 version;
	uint64 steamid;
	uint32 appid;
	uint32 externalIP;
	uint32 internalIP;
	uint32 ownershipFlags;
	time_t ticketGeneration;
	time_t ticketExpiration;
	uint16 licenseCount;
	uint32 *pLicense;
	uint16 dlcCount;
	DLCInfo *pDLC;
	uint16 reserved;
	unsigned char signature[128];
	bool bValid;
};

class AuthBlob /* Concept and CBlob adapted from SteamTools; Ticket Information from OpenSteamWorks. */
{
public:
	AuthBlob(const void *pAuthTicket, int cbAuthTicket)
	{
		CBlob blob(pAuthTicket, cbAuthTicket);
		this->pGCTokenSection = new GCTokenSection(blob);
		this->pSessionSection = new SessionSection(blob);
		this->pOwnershipSection = new OwnershipSection(blob);
		
		this->bExpectedTicket = (this->pGCTokenSection->IsValid() && this->pSessionSection->IsValid() && this->pOwnershipSection->IsValid());
	}
	
	~AuthBlob()
	{
		delete pGCTokenSection;
		delete pSessionSection;
		delete pOwnershipSection;
	}
public:
	GCTokenSection *pGCTokenSection;
	SessionSection *pSessionSection;
	OwnershipSection *pOwnershipSection;
	bool bExpectedTicket;
};
