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
#include "swmemutils.h"
#include "am-utility.h"

void *SteamWorksMemUtils::ResolveSymbolInt(void *pBase, const char *pSymbol)
{
#if defined PLATFORM_POSIX
	if (pSymbol[0] == '@')
	{
		Dl_info info;
		if (dladdr(pBase, &info) == 0)
		{
			return 0;
		}

		void *handle = dlopen(info.dli_fname, RTLD_NOW);
		if (handle == NULL)
		{
			return 0;
		}

		void *pAddress = memutils->ResolveSymbol(handle, &pSymbol[1]);
		dlclose(handle);

		return pAddress;
	}
#endif

	return memutils->FindPattern(pBase, pSymbol, strlen(pSymbol)); /* strlen ??? lol. */
}

size_t SteamWorksMemUtils::GetOffsetFromVTable(void *pInterface, void *pToFindFunc, const char *pClassSig = NULL, size_t version = 0)
{
	void *pEndOfTable = NULL;
	if (pClassSig != NULL)
	{
		size_t len = strlen(pClassSig) + 6;
		ke::AutoArray<char> endoftable = new char[len+1];
		snprintf(*endoftable, len, pClassSig, 'I', version);
		pEndOfTable = this->ResolveSymbolInt(pInterface, *endoftable);
	}

	void **pVTable = (void **)pInterface[0];
	/* 2000 is used as a guard, if there's a class inheriting this many functions, move to a new product. */
	for (size_t iter = 2; iter < 2000; ++iter)
	{
		void *pFunc = pVTable[iter];
		if (pEndOfTable != NULL && pEndOfTable == pFunc)
		{
			break;
		}

		if (pFunc != pToFindFunc)
		{
			continue;
		}

		return (iter - 2);
	}

	return 0;
}
