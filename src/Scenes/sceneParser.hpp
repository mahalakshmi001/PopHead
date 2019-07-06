#pragma once

#include "Utilities/xml.hpp"
#include "EntityComponentSystem/entity.hpp"
#include <string>
#include <vector>

namespace ph{

class SceneParser
{
public:
	SceneParser(GameData* const, Entity& root, const std::string fileName);
private:
	void loadResources(const Xml& sceneSourceCode);
	void loadTextures(const Xml& loadingNode);

	void loadMusic(const Xml& sceneSourceCode);

	void loadScene(const Xml& sceneSourceCode);
	void loadMap(const Xml& rootNode);
	void loadPlayer(const Xml& rootNode);
	void loadGroups(const Xml& rootNode);
	void loadNpcGroup(const Xml& npcGroupNode);
	void loadEnemiesGroup(const Xml& enemyGroupNode);
	void loadZombies(const std::vector<Xml>& zombieNodes);
	void loadSpawnersGroup(const Xml& spawnerGroupNode);

	auto getPositionAttribute(const Xml& objectNode) const -> const sf::Vector2f;

private:
	Entity& mRoot;
	GameData* const mGameData;
};

}