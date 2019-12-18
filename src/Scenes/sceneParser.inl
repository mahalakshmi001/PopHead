#include "sceneParser.hpp"
#include "Utilities/xml.hpp"
#include "Logs/logs.hpp"
#include "gameData.hpp"
#include "Renderer/renderer.hpp"

namespace ph {

template<typename GuiParser, typename MapParser, typename AudioParser, typename EnttParser>
SceneParser<GuiParser, MapParser, AudioParser, typename EnttParser>
	::SceneParser(GameData* const gameData, CutSceneManager& cutSceneManager, EntitiesTemplateStorage& templateStorage,
                  entt::registry& gameRegistry, const std::string& sceneFileName, TextureHolder& textureHolder)
{
	PH_LOG_INFO("Scene linking file (" + sceneFileName + ") is being parsed.");

	// TODO: place it somewhere else
	textureHolder.load("textures/map/FULL_DESERT_TILESET_WIP.png");

	Xml sceneFile;
	sceneFile.loadFromFile(sceneFileName);
	const auto sceneLinksNode = sceneFile.getChild("scenelinks");

	parseEcsEntities(sceneLinksNode, templateStorage, gameRegistry, textureHolder);
	parseMap(sceneLinksNode, gameData->getAIManager(), gameRegistry, templateStorage, textureHolder);
	parse<GuiParser>(gameData, sceneLinksNode, "gui");	
	parse<AudioParser>(gameData, sceneLinksNode, "audio");
	parseAmbientLight(sceneLinksNode);
	//parseGameObjects(gameData, root, cutSceneManager, sceneLinksNode);
}

template<typename GuiParser, typename MapParser/*, typename GameObjectsParser*/, typename AudioParser, typename EnttParser>
template<typename Parser>
void SceneParser<GuiParser, MapParser/*, GameObjectsParser*/, AudioParser, typename EnttParser>
	::parse(GameData* const gameData, const Xml& sceneLinksNode, const std::string& categoryName)
{
	const auto categoryNode = sceneLinksNode.getChildren(categoryName);
	if (categoryNode.size() == 1)
	{
		const std::string categoryFilePath = "scenes/" + categoryName + "/" + categoryNode[0].getAttribute("filename").toString();
		Parser categoryParser;
		categoryParser.parseFile(gameData, categoryFilePath);
	}
}


template<typename GuiParser, typename MapParser, typename AudioParser, typename EnttParser>
void SceneParser<GuiParser, MapParser, AudioParser, EnttParser>
::parseEcsEntities(const Xml& sceneLinksNode, EntitiesTemplateStorage& templateStorage, entt::registry& gameRegistry,
                   TextureHolder& textureHolder)
{
	const auto entitiesNode = sceneLinksNode.getChildren("ecsObjects");
	if (entitiesNode.size() == 1)
	{
		const std::string entitiesFilePath = "scenes/ecs/" + entitiesNode[0].getAttribute("filename").toString();
		EnttParser parser;
		parser.parseFile(entitiesFilePath, templateStorage, gameRegistry, textureHolder);
	}
}

template<typename GuiParser, typename MapParser, typename AudioParser, typename EnttParser>
void SceneParser<GuiParser, MapParser, AudioParser, EnttParser>::parseAmbientLight(const Xml& sceneLinksNode)
{
	const auto ambientLightNode = sceneLinksNode.getChild("ambientLight");
	sf::Color color = ambientLightNode.getAttribute("color").toColor();
	Renderer::setAmbientLightColor(color);
}

template<typename GuiParser, typename MapParser, typename AudioParser, typename EnttParser>
void SceneParser<GuiParser, MapParser, AudioParser, EnttParser>::parseMap(const Xml& sceneLinksNode, AIManager& aiManager, entt::registry& gameRegistry, EntitiesTemplateStorage& templates, TextureHolder& textures)
{
	const auto categoryNode = sceneLinksNode.getChildren("map");
	if (categoryNode.size() == 1)
	{
		const std::string filePath = "scenes/map/" + categoryNode[0].getAttribute("filename").toString();
		MapParser mapParser;
		mapParser.parseFile(filePath, aiManager, gameRegistry, templates, textures);
		//if (mapParser.loadedPlayer())
		//	aiManager.setIsPlayerOnScene(true);
		//else
		//	aiManager.setIsPlayerOnScene(false);
	}
}

}
