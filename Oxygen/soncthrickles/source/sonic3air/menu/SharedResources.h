/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2022 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxmedia.h>
#include "oxygen/drawing/DrawerTexture.h"


namespace global
{
	struct ZoneActPreviewKey
	{
		uint8 mZone = 0;
		uint8 mAct = 0;
		uint8 mImage = 0;

		inline bool operator<(const ZoneActPreviewKey& other) const  { return (mZone != other.mZone) ? (mZone < other.mZone) : (mAct != other.mAct) ? (mAct < other.mAct) : (mImage < other.mImage); }
	};

	extern Font mFont3;
	extern Font mFont3Pure;
	extern Font mFont4;
	extern Font mFont5;
	extern Font mFont7;
	extern Font mFont10;
	extern Font mFont18;

	extern DrawerTexture mGameLogo;
	extern DrawerTexture mMainMenuBackgroundLeft;
	extern DrawerTexture mMainMenuBackgroundSeparator;
	extern DrawerTexture mDataSelectBackground;
	extern DrawerTexture mDataSelectAltBackground;
	extern DrawerTexture mLevelSelectBackground;
	extern DrawerTexture mPreviewBorder;
	extern DrawerTexture mOptionsTopBar;
	extern DrawerTexture mCharactersIcon[3];
	extern DrawerTexture mCharSelectionBox;
	extern DrawerTexture mAchievementsFrame;
	extern DrawerTexture mPauseScreenUpperBG;
	extern DrawerTexture mPauseScreenLowerBG;
	extern DrawerTexture mPauseScreenDialog2BG;
	extern DrawerTexture mPauseScreenDialog3BG;
	extern DrawerTexture mTimeAttackResultsBG;

	extern std::map<ZoneActPreviewKey, DrawerTexture> mZoneActPreview;
	extern std::map<uint32, DrawerTexture> mAchievementImage;
	extern std::map<uint32, DrawerTexture> mSecretImage;

	void loadSharedResources();
}
