/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2022 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/EngineDelegate.h"
#include "sonic3air/audio/AudioOut.h"
#include "sonic3air/menu/GameApp.h"
#include "sonic3air/menu/SharedResources.h"
#include "sonic3air/version.inc"
#ifndef ENDUSER
	#include "sonic3air/generator/ResourceScriptGenerator.h"
#endif

#include "oxygen/application/GameProfile.h"
#include "oxygen/base/CrashHandler.h"
#include "oxygen/simulation/CodeExec.h"
#include "oxygen/simulation/EmulatorInterface.h"

#include <lemon/program/Program.h>
#include <lemon/runtime/provider/NativizedOpcodeProvider.h>


namespace lemon
{
	// Forward declaration of the nativized code lookup builder function
	extern void createNativizedCodeLookup(Nativizer::LookupDictionary& dict);
}


const EngineDelegateInterface::AppMetaData& EngineDelegate::getAppMetaData()
{
	if (mAppMetaData.mTitle.empty())
	{
		mAppMetaData.mTitle = "Sonic 3 A.I.R.";
		mAppMetaData.mIconFile = L"data/images/icon.png";
		mAppMetaData.mWindowsIconResource = 101;
		mAppMetaData.mBuildVersion = BUILD_STRING;
		mAppMetaData.mAppDataFolder = L"Sonic3AIR";
	}
	return mAppMetaData;
}

GuiBase& EngineDelegate::createGameApp()
{
	return *new GameApp();
}

AudioOutBase& EngineDelegate::createAudioOut()
{
	return *new AudioOut();
}

bool EngineDelegate::onEnginePreStartup()
{
	CrashHandler::setApplicationInfo(std::string("Sonic 3 A.I.R. v") + BUILD_STRING);

#ifdef ENDUSER
	// Sanity check if the game is even extracted
	{
		// One of these two files must exist
	#if defined(PLATFORM_MAC)
		Configuration& config = Configuration::instance();
		const bool check = FTX::FileSystem->exists(config.mGameDataPath + L"/gamedata.bin");
	#else
		const bool check = (FTX::FileSystem->exists(L"data/content.json") || FTX::FileSystem->exists(L"data/gamedata.bin"));
	#endif
		if (!check)
		{
		#ifdef PLATFORM_WINDOWS
			RMX_ERROR("Seems like you launched the Sonic3AIR.exe from inside the downloaded ZIP file.\n\nMake sure to first extract the ZIP somewhere like on your desktop, then start the Sonic3AIR.exe in the extracted folder.", );
		#else
			RMX_ERROR("Seems like you launched the Sonic3AIR executable from inside the downloaded ZIP file.\n\nMake sure to first extract the ZIP somewhere like on your desktop, then start the Sonic3AIR executable in the extracted folder.", );
		#endif
			return false;
		}
	}
#endif

	return true;
}

bool EngineDelegate::setupCustomGameProfile()
{
#ifdef ENDUSER
	// Setup game profile data -- this is done so that no oxygenproject.json is needed for the end-user version of S3AIR
	GameProfile& gameProfile = GameProfile::instance();

	gameProfile.mAsmStackRange.first = 0xfffffd00;
	gameProfile.mAsmStackRange.second = 0xfffffe00;

	gameProfile.mRomCheck.mSize = 0x400000;
	gameProfile.mRomCheck.mOverwrites.emplace_back(0x2001f0, 0x4a);
	gameProfile.mRomCheck.mChecksum = 0x0c06aa82;

	gameProfile.mDataPackages.clear();
	gameProfile.mDataPackages.emplace_back(L"enginedata.bin",    true);
	gameProfile.mDataPackages.emplace_back(L"gamedata.bin",      true);
	gameProfile.mDataPackages.emplace_back(L"audiodata.bin",     true);
	gameProfile.mDataPackages.emplace_back(L"audioremaster.bin", false);	// Optional package
#else

	// Just load from the oxygenproject.json file
	GameProfile::instance().loadOxygenProjectFromFile(L"oxygenproject.json");
#endif

	// Return true, so the engine won't load the oxygenprofile.json by itself
	return true;
}

void EngineDelegate::startupGame()
{
	mGame.startup();
}

void EngineDelegate::shutdownGame()
{
	mGame.shutdown();
}

void EngineDelegate::updateGame(float timeElapsed)
{
	mGame.update(timeElapsed);
}

void EngineDelegate::registerScriptBindings(lemon::Module& module)
{
	Game::instance().registerScriptBindings(module);
}

void EngineDelegate::registerNativizedCode(lemon::Program& program)
{
	static lemon::NativizedOpcodeProvider instance(&lemon::createNativizedCodeLookup);
	program.mNativizedOpcodeProvider = instance.isValid() ? &instance : nullptr;
}

void EngineDelegate::onRuntimeInit(CodeExec& codeExec)
{
	// Enable debug mode
	if (mGame.isDebugModeActive())
	{
		codeExec.getEmulatorInterface().writeMemory16(0xffffffda, 1);
	}

#ifndef ENDUSER
	// Generation of scripts from data in ROM or another source
	//ResourceScriptGenerator::generateLevelObjectTableScript(codeExec);
	//ResourceScriptGenerator::generateLevelRingsTableScript(codeExec);
	//ResourceScriptGenerator::convertLevelObjectsBinToScript(L"E:/Projects/Workspace/Oxygen/_SonLVL/skdisasm-master/Levels/AIZ/Object Pos/1.bin", L"output_objects.lemon");
	//ResourceScriptGenerator::convertLevelRingsBinToScript(L"E:/Projects/Workspace/Oxygen/_SonLVL/skdisasm-master/Levels/AIZ/Ring Pos/1.bin", L"output_rings.lemon");
#endif
}

void EngineDelegate::onPreFrameUpdate()
{
	mGame.onPreUpdateFrame();
}

void EngineDelegate::onPostFrameUpdate()
{
	mGame.onPostUpdateFrame();
}

void EngineDelegate::onControlsUpdate()
{
	mGame.onUpdateControls();
}

void EngineDelegate::onPreSaveStateLoad()
{
	mGame.setCurrentMode(Game::Mode::UNDEFINED);
}

void EngineDelegate::onApplicationLostFocus()
{
	mGame.enableGamePauseByApplication();
}

bool EngineDelegate::mayLoadScriptMods()
{
	return allowModdedData();
}

bool EngineDelegate::allowModdedData()
{
	// TODO: How about Competition Mode Time Attack?
	return (mGame.getCurrentMode() != Game::Mode::TIME_ATTACK);
}

bool EngineDelegate::useDeveloperFeatures()
{
#if defined(ENDUSER)
	return mConfiguration.mDevMode;
#else
	return true;
#endif
}

void EngineDelegate::onGameRecordingHeaderLoaded(const std::string& buildString, const std::vector<uint8>& buffer)
{
	mGame.onGameRecordingHeaderLoaded(buildString, buffer);
}

void EngineDelegate::onGameRecordingHeaderSave(std::vector<uint8>& buffer)
{
	mGame.onGameRecordingHeaderSave(buffer);
}

Font& EngineDelegate::getDebugFont(int size)
{
	return (size >= 10) ? global::mFont10 : global::mFont3;
}

void EngineDelegate::fillDebugVisualization(Bitmap& bitmap, int& mode)
{
	mGame.fillDebugVisualization(bitmap, mode);
}
