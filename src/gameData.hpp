#pragma once

#include "Audio/Music/musicPlayer.hpp"
#include "Audio/Sound/soundPlayer.hpp"
#include "Renderer/renderer.hpp"
#include "AI/aiManager.hpp"
#include "Scenes/sceneManager.hpp"
#include "Map/map.hpp"
#include "Resources/resourceHolder.hpp"
#include "Physics/physicsEngine.hpp"
#include "Terminal/terminal.hpp"
#include "EfficiencyRegister/efficiencyRegister.hpp"
#include "Gui/GUI.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

namespace ph {

class GameCloser
{
public:
	void closeGame();
	bool shouldGameBeClosed();

private:
	bool mShouldGameBeClosed = false;
};

/// GameData is holder for observer pointers to Game Modules.

class GameData
{
public:
	GameData()
	:GameData(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) {}
	
	GameData(
		sf::Window* const window,
		SoundPlayer* const soundPlayer,
		MusicPlayer* const musicPlayer,
		TextureHolder* const textures,
		FontHolder* const fonts,
		ShaderHolder* const shaders,
		AIManager* const aiManager,
		SceneManager* const sceneManager,
		Map* const map,
		Renderer* const renderer,
		PhysicsEngine* const physicsEngine,
		Terminal* const Terminal,
		EfficiencyRegister* const efficiencyRegister,
		GUI* const Gui
	)
		:mWindow(window)
		,mSoundPlayer{soundPlayer}
		,mMusicPlayer{musicPlayer}
		,mTextures{textures}
		,mFonts{fonts}
		,mShaders{shaders}
		,mAIMangager(aiManager)
		,mSceneManager{sceneManager}
		,mMap(map)
		,mRenderer{renderer}
		,mPhysicsEngine{physicsEngine}
		,mTerminal{Terminal}
		,mEfficiencyRegister{efficiencyRegister}
		,mGui(Gui)
		,mGameCloser()
	{
	}
	
	auto getWindow() const -> sf::Window& { return *mWindow; }
	auto getSoundPlayer() const -> SoundPlayer& { return *mSoundPlayer; }
	auto getMusicPlayer() const -> MusicPlayer& { return *mMusicPlayer; }
	auto getTextures() const -> TextureHolder& { return *mTextures; }
	auto getFonts()	const -> FontHolder& { return *mFonts; }
	auto getShaders() const -> ShaderHolder& { return *mShaders; }
	auto getAIManager() const -> AIManager& { return *mAIMangager; }
	auto getSceneManager() const -> SceneManager& { return *mSceneManager; }
	auto getMap() const -> Map& { return *mMap; }
	auto getRenderer() const -> Renderer& { return *mRenderer; }
	auto getPhysicsEngine()	const -> PhysicsEngine& { return *mPhysicsEngine; }
	auto getTerminal() const -> Terminal& { return *mTerminal; }
	auto getEfficiencyRegister() const -> EfficiencyRegister& { return *mEfficiencyRegister; }
	auto getGui() const -> GUI& { return *mGui; }
	auto getGameCloser() -> GameCloser& { return mGameCloser; }

private:
	sf::Window* const mWindow;
	SoundPlayer* const mSoundPlayer;
	MusicPlayer* const mMusicPlayer;
	TextureHolder* const mTextures;
	FontHolder* const mFonts;
	ShaderHolder* const mShaders;
	AIManager* const mAIMangager;
	SceneManager* const mSceneManager;
	Map* const mMap;
	Renderer* const mRenderer;
	PhysicsEngine* const mPhysicsEngine;
	Terminal* const mTerminal;
	EfficiencyRegister* const mEfficiencyRegister;
	GUI* const mGui;
	GameCloser mGameCloser;
};

}
