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
#include "smsdk_ext.h"

class SteamWorksMemUtils
{
	public:
		void *ResolveSymbolInt(void *pBase, const char *pSymbol);
		size_t GetOffsetFromVTable(void *pInterface, void *pToFindFunc, const char *pClassSig = NULL, size_t version = 0);
};

#include "extension.h"
