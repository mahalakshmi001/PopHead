#include "startGameCutscene.hpp"
#include "GameObjects/gameObject.hpp"
#include "GameObjects/DrawableGameObjects/car.hpp"
#include "Renderer/camera.hpp"
#include "Audio/Sound/soundPlayer.hpp"
#include "Audio/Music/musicPlayer.hpp"
#include "Gui/gui.hpp"
#include "GameObjects/DrawableGameObjects/Characters/Npcs/crawlingNpc.hpp"
#include "gameData.hpp"

namespace ph {

StartGameCutScene::StartGameCutScene(GameObject& root, Camera& camera, SoundPlayer& soundPlayer, MusicPlayer& musicPlayer,
	GUI& gui, GameData* const gameData)
	:CutScene(root)
	,mCamera(camera)
	,mSoundPlayer(soundPlayer)
	,mMusicPlayer(musicPlayer)
	,mGui(gui)
	,mGameData(gameData)
	,mHasStartedToSlowDown(false)
	,mHasChangedTheMusicToMenuTheme(false)
	,mWasPlayerCreated(false)
	,mHasPlayerTurnedToNpc(false)
	,mWereZombieSpawned(false)
	,mHasChangedMusicToZombieAttackTheme(false)
{
	auto* car = dynamic_cast<Car*>(root.getChild("LAYER_standingObjects")->getChild("car"));
	car->setVelocity(120);
}

void StartGameCutScene::update(const sf::Time delta)
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		closeCutScene();

	const float cutsceneTimeInSeconds = mClock.getElapsedTime().asSeconds();

	auto& car = dynamic_cast<Car&>(*mRoot.getChild("LAYER_standingObjects")->getChild("car"));
	mCamera.setCenter(car.getPosition() + sf::Vector2f(15, 10));

	if(cutsceneTimeInSeconds < 5)
		car.speedUp();

	if(cutsceneTimeInSeconds < 23)
		updateNarrativeSubtitles(cutsceneTimeInSeconds, car);

	if(car.getPosition().x > 4965 && !mHasStartedToSlowDown) {
		mSoundPlayer.playAmbientSound("sounds/carTireScreech.ogg");
		mHasStartedToSlowDown = true;
	}

	if(car.getPosition().x > 5000)
		car.slowDown();

	if(cutsceneTimeInSeconds > 19 && !mHasChangedTheMusicToMenuTheme) {
		mMusicPlayer.play("music/Menu.ogg");
		mHasChangedTheMusicToMenuTheme = true;
	}
	
	if(cutsceneTimeInSeconds > 23 && !mWasPlayerCreated)
		createPlayer();

	if(cutsceneTimeInSeconds > 24 && !mHasPlayerTurnedToNpc) {
		rotatePlayer();
		mCamera.setSize({320, 240});
	}

	if(cutsceneTimeInSeconds > 25 && cutsceneTimeInSeconds < 30)
		updateSpeech(cutsceneTimeInSeconds);

	if(cutsceneTimeInSeconds > 27.5) {
		auto* crawlingNpc = dynamic_cast<CrawlingNpc*>(mRoot.getChild("crawlingNpc"));
		crawlingNpc->die();
	}

	if(cutsceneTimeInSeconds > 32 && cutsceneTimeInSeconds < 39)
		rotateAround(cutsceneTimeInSeconds);

	if(cutsceneTimeInSeconds > 37.5 && !mHasChangedMusicToZombieAttackTheme)
		mMusicPlayer.play("music/zombieAttack.ogg");

	if(cutsceneTimeInSeconds > 39)
		lookSouth();

	if(cutsceneTimeInSeconds > 40)
		sayFuck(cutsceneTimeInSeconds);

	if(cutsceneTimeInSeconds > 43 && !mWereZombieSpawned) {
		spawnZombies();
		mWereZombieSpawned = true;
		mCamera.setSize({640, 480});
	}

	if(cutsceneTimeInSeconds > 45)
		closeCutScene();
}

void StartGameCutScene::updateNarrativeSubtitles(const float cutsceneTimeInSeconds, Car& car)
{
	auto canvas = mGui.getInterface("labels")->getWidget("canvas");

	if(cutsceneTimeInSeconds < 4) {
		canvas->getWidget("place")->hide();
		canvas->getWidget("time")->hide();
		canvas->getWidget("velocity")->hide();
		canvas->getWidget("speechBubble")->hide();
	}
	else if(cutsceneTimeInSeconds > 4 && cutsceneTimeInSeconds < 8) {
		canvas->getWidget("place")->show();
	}
	else if(cutsceneTimeInSeconds > 8 && cutsceneTimeInSeconds < 10) {
		canvas->getWidget("place")->hide();
	}
	else if(cutsceneTimeInSeconds > 10 && cutsceneTimeInSeconds < 15) {
		canvas->getWidget("time")->show();
	}
	else if(cutsceneTimeInSeconds > 15 && cutsceneTimeInSeconds < 17) {
		canvas->getWidget("time")->hide();
	}
	else if(cutsceneTimeInSeconds > 17 && cutsceneTimeInSeconds < 22) {
		auto velocityWidget = dynamic_cast<TextWidget*>(canvas->getWidget("velocity"));
		velocityWidget->show();
		int velocity = static_cast<int>(car.getVelocity() / 4.7);
		velocityWidget->setString(std::to_string(velocity) + " MPH");
	}
	else if(cutsceneTimeInSeconds > 22 && cutsceneTimeInSeconds < 24.5) {
		canvas->getWidget("velocity")->hide();
	}
}

void StartGameCutScene::createPlayer()
{
	auto playerNpc = std::make_unique<Npc>(mGameData, "playerNpc");
	playerNpc->setPosition({5640, 400});
	mRoot.addChild(std::move(playerNpc));
	mWasPlayerCreated = true;
}

void StartGameCutScene::rotatePlayer()
{
	auto& playerNpc = dynamic_cast<Character&>(*mRoot.getChild("playerNpc"));
	playerNpc.setAnimationState("rightUp");
	mHasPlayerTurnedToNpc = true;
}

void StartGameCutScene::updateSpeech(const float cutsceneTimeInSeconds)
{
	auto canvas = mGui.getInterface("labels")->getWidget("canvas");
	auto speechBubble = canvas->getWidget("speechBubble");

	if(cutsceneTimeInSeconds > 25 && cutsceneTimeInSeconds < 29) {
		speechBubble->show();
		speechBubble->getWidget("speech2")->hide();
		speechBubble->getWidget("speech3")->hide();
	}
	else if(cutsceneTimeInSeconds > 29 && cutsceneTimeInSeconds < 31) {
		speechBubble->getWidget("speech1")->hide();
		speechBubble->getWidget("speech1b")->hide();
		speechBubble->getWidget("speech2")->show();
	}
}

void StartGameCutScene::rotateAround(const float cutsceneTimeInSeconds)
{
	auto& playerNpc = dynamic_cast<Character&>(*mRoot.getChild("playerNpc"));
	auto& animation = playerNpc.getAnimation();

	if(cutsceneTimeInSeconds > 32 && cutsceneTimeInSeconds < 33)
		animation.changeState("right");
	else if(cutsceneTimeInSeconds > 33 && cutsceneTimeInSeconds < 34)
		animation.changeState("down");
	else if(cutsceneTimeInSeconds > 34 && cutsceneTimeInSeconds < 35)
		animation.changeState("left");
	else if(cutsceneTimeInSeconds > 35 && cutsceneTimeInSeconds < 36)
		animation.changeState("leftUp");
	else if(cutsceneTimeInSeconds > 36 && cutsceneTimeInSeconds < 37)
		animation.changeState("up");
	else if(cutsceneTimeInSeconds > 37 && cutsceneTimeInSeconds < 38)
		animation.changeState("rightUp");
	else if(cutsceneTimeInSeconds > 38 && cutsceneTimeInSeconds < 39)
		animation.changeState("right");
}

void StartGameCutScene::lookSouth()
{
	auto& playerNpc = dynamic_cast<Character&>(*mRoot.getChild("playerNpc"));
	auto& animation = playerNpc.getAnimation();
	animation.changeState("down");
}

void StartGameCutScene::sayFuck(const float cutsceneTimeInSeconds)
{
	auto canvas = mGui.getInterface("labels")->getWidget("canvas");
	auto speechBubble = canvas->getWidget("speechBubble");

	if(cutsceneTimeInSeconds > 40 && cutsceneTimeInSeconds < 43) {
		speechBubble->getWidget("speech2")->hide();
		speechBubble->getWidget("speech3")->show();
	}
	else if(cutsceneTimeInSeconds > 43 && cutsceneTimeInSeconds < 44)
		speechBubble->hide();
}

void StartGameCutScene::spawnZombies()
{
	createZombie({5670, 270});
}

void StartGameCutScene::createZombie(const sf::Vector2f position)
{
	auto zombie = std::make_unique<Npc>(mGameData);
	zombie->setPosition(position);
	zombie->getSprite().setTexture(mGameData->getTextures().get("textures/characters/zombieFullAnimation.png"));
	mRoot.addChild(std::move(zombie));
}

void StartGameCutScene::closeCutScene()
{
	mGameData->getSceneManager().replaceScene("scenes/gateAreaMap.xml");
	mIsActive = false;
}

}