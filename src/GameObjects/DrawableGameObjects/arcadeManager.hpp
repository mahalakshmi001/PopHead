#pragma once

#include "GameObjects/gameObject.hpp"

#include <map>

namespace ph {

class GUI;

class ArcadeManager : public GameObject
{
public:
	ArcadeManager(GUI& gui);
	void updateCurrent(const sf::Time delta) override;

private:
	void updateClock();
	void updateWave();
	void updateEnemies();
	bool shouldCreateNewWave();
	void createNextWave();
	void updateCounters();

private:
	GUI& mGui;
	sf::Clock mArcadeClock;
	sf::Clock mTimeOnCurrentWave;
	std::map<int, std::pair<int, sf::Time>> mWavesMap;
	int mCurrentWave;
	int mEnemiesLeft;
};

}