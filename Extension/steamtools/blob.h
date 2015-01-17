/*
 * ======================================================
 * OpenCoordinator
 * Copyright (C) 2011 The OpenCoordinator Team.
 * All rights reserved.
 * ======================================================
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 * claim that you wrote the original software. If you use this software in a 
 * product, an acknowledgment in the product documentation would be 
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
 
/* Modified for SteamWorks, taken from SteamTools. */

#pragma once

class CBlob
{
public:
	CBlob(const void *pMessage, size_t iSize)
		: m_pMessage(pMessage)
		, m_iSize(iSize)
		, m_iCurrentOffset(0)
	{};

private:
	const void *m_pMessage;
	size_t m_iSize;
	size_t m_iCurrentOffset;

public:
	inline size_t GetPosition()
	{
		return m_iCurrentOffset;
	}

	inline size_t AdvancePosition(size_t iAdditionalOffset)
	{
		m_iCurrentOffset += iAdditionalOffset;
		return m_iCurrentOffset;
	}

	inline size_t RewindPosition(size_t iAdditionalNegativeOffset)
	{
		m_iCurrentOffset -= iAdditionalNegativeOffset;
		return m_iCurrentOffset;
	}
	
	inline void SetPosition(size_t iNewOffset)
	{
		m_iCurrentOffset = iNewOffset;
	}

	inline void ResetPosition()
	{
		m_iCurrentOffset = 0;
	}
	
	inline bool CanStillRead(void)
	{
		return (m_iCurrentOffset >= m_iSize);
	}

public:
	template <class T>
	T Read(bool *bError = NULL)
	{
		if ((m_iCurrentOffset + sizeof(T)) > m_iSize)
		{
			if (bError)
				*bError = true;
			return (T)0;
		}

		T tempBuffer = *((T *)((intptr_t)m_pMessage + m_iCurrentOffset));
		m_iCurrentOffset += sizeof(T);

		if (bError)
			*bError = false;

		return tempBuffer;
	}

	template <class T>
	bool Read(T *pOut)
	{
		if ((m_iCurrentOffset + sizeof(T)) > m_iSize)
			return false;

		T *pTempBuffer = (T *)((intptr_t)m_pMessage + m_iCurrentOffset);
		m_iCurrentOffset += sizeof(T);
		memcpy(pOut, pTempBuffer, sizeof(T));

		return true;
	}
	
	template <class T>
	bool Read(T &Out)
	{
		if ((m_iCurrentOffset + sizeof(T)) > m_iSize)
			return false;

		T *pTempBuffer = (T *)((intptr_t)m_pMessage + m_iCurrentOffset);
		m_iCurrentOffset += sizeof(T);
		Out = *pTempBuffer;

		return true;
	}

	bool Read(void *pOut, size_t iLength)
	{
		if ((m_iCurrentOffset + iLength) > m_iSize)
			return false;

		void *pTempBuffer = (void *)((intptr_t)m_pMessage + m_iCurrentOffset);
		m_iCurrentOffset += iLength;
		memcpy(pOut, pTempBuffer, iLength);

		return true;
	}
};