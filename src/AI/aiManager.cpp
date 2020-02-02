#include "aiManager.hpp" 
#include "aStarAlgorithm.hpp"
#include "RandomPathAlgorithm.hpp"
#include <algorithm>
#include <cmath>

namespace ph {

PathMode AIManager::getZombiePath(sf::Vector2f zombiePosition) const
{
	if (!mIsPlayerOnScene || mAIMode == AIMode::zombieAlwaysWalkRandomly)
		return { getRandomPath(zombiePosition) };
	if (mAIMode == AIMode::zombieAlwaysLookForPlayer)
		return { getPath(zombiePosition, mPlayerPosition), true };

	float distanceToPlayer = getDistanceBetweenZombieAndPlayer(zombiePosition);
	constexpr float maximalDistanceFromWhichZombieSeesPlayer = 285.f;
	constexpr float maximalDistanceFromWhichZombieWalksRandom = 350.f;

	if (distanceToPlayer <= maximalDistanceFromWhichZombieSeesPlayer)
		return { getPath(zombiePosition, mPlayerPosition), true };
	else if (distanceToPlayer <= maximalDistanceFromWhichZombieWalksRandom)
		return { getRandomPath(zombiePosition) };
	return { Path() };
}

bool AIManager::shouldZombiePlayAttackAnimation(sf::Vector2f zombiePosition) const
{
	float distanceBetweenZombieAndPlayer = getDistanceBetweenZombieAndPlayer(zombiePosition);
	return distanceBetweenZombieAndPlayer < 25;
}

void AIManager::setPlayerPosition(sf::Vector2f playerPosition) 
{ 
	this->mPlayerPosition = playerPosition; 
	mHasPlayerMovedSinceLastUpdate = true;
}

void AIManager::registerMapSize(sf::Vector2u mapSizeInTiles)
{
	mObstacleGrid = ObstacleGrid(mapSizeInTiles.x, mapSizeInTiles.y);
}

void AIManager::registerObstacle(sf::Vector2f gridPosition)
{
	mObstacleGrid.registerObstacle(static_cast<size_t>(gridPosition.x), static_cast<size_t>(gridPosition.y));
}

void AIManager::update()
{
	mHasPlayerMovedSinceLastUpdate = false;
}

float AIManager::getDistanceBetweenZombieAndPlayer(sf::Vector2f zombiePosition) const
{
	float legX = std::abs(zombiePosition.x - mPlayerPosition.x);
	float legY = std::abs(zombiePosition.y - mPlayerPosition.y);
	float distance = std::hypotf(legX, legY);
	return distance;
}

Path AIManager::getPath(sf::Vector2f startPosition, sf::Vector2f destinationPosition) const
{
	auto dest = toNodePosition(destinationPosition);
	if (mObstacleGrid.isObstacle(dest.x, dest.y))
		dest += sf::Vector2u(1, 1);

	AStarAlgorithm a(mObstacleGrid, toNodePosition(startPosition), dest);
	auto path = a.getPath();
	//if (path.size() > 3)
	//	path.erase(path.cbegin() + 3, path.cend());
	return path;
}

sf::Vector2u AIManager::toNodePosition(sf::Vector2f position) const
{
	return static_cast<sf::Vector2u>(sf::Vector2f(position.x / mTileSize.x, position.y / mTileSize.y));
}

Path AIManager::getRandomPath(sf::Vector2f startPosition) const
{
	RandomPathAlgorithm rpa(mObstacleGrid, toNodePosition(startPosition));
	return rpa.getRandomPath();
}

}
