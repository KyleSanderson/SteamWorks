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
#include "swgamedata.h"

SteamWorksGameData::SteamWorksGameData()
{
	char buffer[PLATFORM_MAX_PATH];
	this->pGameConf = NULL;
	
	/* LoadGameConfigFile will verbosely error out, we need to check this since it's optional. */
	smutils->BuildPath(Path_SM, buffer, sizeof(buffer), "gamedata/steamworks.txt");
	if (libsys->PathExists(buffer) && libsys->IsPathFile(buffer))
	{
		gameconfs->LoadGameConfigFile("steamworks", &this->pGameConf, buffer, sizeof(buffer));
	}
}

SteamWorksGameData::~SteamWorksGameData()
{
	if (this->pGameConf != NULL)
	{
		gameconfs->CloseGameConfigFile(this->pGameConf);
		this->pGameConf = NULL;
	}
}

bool SteamWorksGameData::HasGameData(void) const
{
	return (this->pGameConf != NULL);
}

IGameConfig *SteamWorksGameData::GetGameData(void) const
{
	return this->pGameConf;
}
