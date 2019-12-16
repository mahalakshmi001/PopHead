#include "xmlMapParser.hpp"
#include "Logs/logs.hpp"

#include "ECS/Components/graphicsComponents.hpp"
#include "ECS/Components/physicsComponents.hpp"

#include "AI/aiManager.hpp"
#include "Utilities/xml.hpp"
#include "Utilities/csv.hpp"
#include "Utilities/filePath.hpp"
#include "Utilities/math.hpp"

namespace ph {

void XmlMapParser::parseFile(const std::string& fileName, AIManager& aiManager, entt::registry& gameRegistry, EntitiesTemplateStorage& templates, TextureHolder& textures)
{
	PH_LOG_INFO("Map file (" + fileName + ") is being parsed.");

	mGameRegistry = &gameRegistry;
	mTemplates = &templates;
	mTextures = &textures;
		
	Xml mapFile;
	mapFile.loadFromFile(fileName);
	const Xml mapNode = mapFile.getChild("map");
	checkMapSupport(mapNode);

	GeneralMapInfo generalMapInfo = getGeneralMapInfo(mapNode);
	aiManager.registerMapSize(generalMapInfo.mapSize);

	const std::vector<Xml> tilesetNodes = getTilesetNodes(mapNode);
	const TilesetsData tilesetsData = getTilesetsData(tilesetNodes);
	const std::vector<Xml> layerNodes = getLayerNodes(mapNode);
	
	parserMapLayers(layerNodes, tilesetsData, generalMapInfo);
	createMapBorders(generalMapInfo);
}

void XmlMapParser::checkMapSupport(const Xml& mapNode) const
{
	const std::string orientation = mapNode.getAttribute("orientation").toString();
	if(orientation != "orthogonal")
		PH_EXCEPTION("Used unsupported map orientation: " + orientation);
	const std::string infinite = mapNode.getAttribute("infinite").toString();
	if(infinite != "0")
		PH_EXCEPTION("Infinite maps are not supported");
}

auto XmlMapParser::getGeneralMapInfo(const Xml& mapNode) const -> GeneralMapInfo
{
	return { getMapSize(mapNode), getTileSize(mapNode) };
}

sf::Vector2u XmlMapParser::getMapSize(const Xml& mapNode) const
{
	return sf::Vector2u(
		mapNode.getAttribute("width").toUnsigned(),
		mapNode.getAttribute("height").toUnsigned()
	);
}

sf::Vector2u XmlMapParser::getTileSize(const Xml& mapNode) const
{
	return sf::Vector2u(
		mapNode.getAttribute("tilewidth").toUnsigned(),
		mapNode.getAttribute("tileheight").toUnsigned()
	);
}

std::vector<Xml> XmlMapParser::getTilesetNodes(const Xml& mapNode) const
{
	const std::vector<Xml> tilesetNodes = mapNode.getChildren("tileset");
	if(tilesetNodes.size() == 0)
		PH_LOG_WARNING("Map doesn't have any tilesets");
	return tilesetNodes;
}

auto XmlMapParser::getTilesetsData(const std::vector<Xml>& tilesetNodes) const -> const TilesetsData
{
	TilesetsData tilesets;
	tilesets.firstGlobalTileIds.reserve(tilesetNodes.size());
	tilesets.tileCounts.reserve(tilesetNodes.size());
	tilesets.columnsCounts.reserve(tilesetNodes.size());
	
	for(Xml tilesetNode : tilesetNodes) {
		const unsigned firstGlobalTileId = tilesetNode.getAttribute("firstgid").toUnsigned();
		tilesets.firstGlobalTileIds.push_back(firstGlobalTileId);
		if(tilesetNode.hasAttribute("source")) {
			std::string tilesetNodeSource = tilesetNode.getAttribute("source").toString();
			tilesetNodeSource = FilePath::toFilename(tilesetNodeSource, '/');
			PH_LOG_INFO("Detected not embedded tileset in Map: " + tilesetNodeSource);
			Xml tilesetDocument;
			tilesetDocument.loadFromFile(tilesetNodeSource);
			tilesetNode = tilesetDocument.getChild("tileset");
		}

		tilesets.tileCounts.push_back(tilesetNode.getAttribute("tilecount").toUnsigned());
		tilesets.columnsCounts.push_back(tilesetNode.getAttribute("columns").toUnsigned());
		const Xml imageNode = tilesetNode.getChild("image");
		tilesets.tilesetFileName = FilePath::toFilename(imageNode.getAttribute("source").toString(), '/');
		const std::vector<Xml> tileNodes = tilesetNode.getChildren("tile");
		TilesData tilesData = getTilesData(tileNodes);
		tilesData.firstGlobalTileId = firstGlobalTileId;
		tilesets.tilesData.push_back(tilesData);
	}

	return tilesets;
}

auto XmlMapParser::getTilesData(const std::vector<Xml>& tileNodes) const -> TilesData
{
	TilesData tilesData{};
	tilesData.ids.reserve(tileNodes.size());
	tilesData.bounds.reserve(tileNodes.size());
	for(const Xml& tileNode : tileNodes) {
		tilesData.ids.push_back(tileNode.getAttribute("id").toUnsigned());
		const Xml objectGroupNode = tileNode.getChild("objectgroup");
		const Xml objectNode = objectGroupNode.getChild("object");
		const sf::FloatRect bounds(
			objectNode.getAttribute("x").toFloat(),
			objectNode.getAttribute("y").toFloat(),
			objectNode.hasAttribute("width") ? objectNode.getAttribute("width").toFloat() : 0.f,
			objectNode.hasAttribute("height") ? objectNode.getAttribute("height").toFloat() : 0.f
		);
		
		tilesData.bounds.push_back(bounds);
	}
	return tilesData;
}

std::vector<Xml> XmlMapParser::getLayerNodes(const Xml& mapNode) const
{
	const std::vector<Xml> layerNodes = mapNode.getChildren("layer");
	if(layerNodes.size() == 0)
		PH_LOG_WARNING("Map doesn't have any layers");
	return layerNodes;
}

void XmlMapParser::parserMapLayers(const std::vector<Xml>& layerNodes, const TilesetsData& tilesets, const GeneralMapInfo& info)
{
	unsigned char z = 200;
	for (const Xml& layerNode : layerNodes)
	{
		const Xml dataNode = layerNode.getChild("data");
		const auto globalIds = toGlobalTileIds(dataNode);
		createLayer(globalIds, tilesets, info, z);
		--z;
	}
}

std::vector<unsigned> XmlMapParser::toGlobalTileIds(const Xml& dataNode) const
{
	const std::string encoding = dataNode.getAttribute("encoding").toString();
	if(encoding == "csv")
		return Csv::toUnsigneds(dataNode.toString());
	PH_EXCEPTION("Used unsupported data encoding: " + encoding);
}

void XmlMapParser::createLayer(const std::vector<unsigned>& globalTileIds, const TilesetsData& tilesets, const GeneralMapInfo& info, unsigned char z)
{
	const static std::string pathToTilesetsDirectory = "textures/map/";
	Texture& texture = mTextures->get(pathToTilesetsDirectory + tilesets.tilesetFileName);

	for (std::size_t tileIndexInMap = 0; tileIndexInMap < globalTileIds.size(); ++tileIndexInMap) {
		// WARNING: this code assumes int has 4 bytes

		const unsigned bitsInByte = 8;
		const unsigned flippedHorizontally = 1u << (sizeof(unsigned) * bitsInByte - 1);
		const unsigned flippedVertically = 1u << (sizeof(unsigned) * bitsInByte - 2);
		const unsigned flippedDiagonally = 1u << (sizeof(unsigned) * bitsInByte - 3);

		const bool isHorizontallyFlipped = globalTileIds[tileIndexInMap] & flippedHorizontally;
		const bool isVerticallyFlipped = globalTileIds[tileIndexInMap] & flippedVertically;
		const bool isDiagonallyFlipped = globalTileIds[tileIndexInMap] & flippedDiagonally;

		const unsigned globalTileId = globalTileIds[tileIndexInMap] & (~(flippedHorizontally | flippedVertically | flippedDiagonally));

		if (hasTile(globalTileId)) {
			const std::size_t tilesetIndex = findTilesetIndex(globalTileId, tilesets);
			if (tilesetIndex == std::string::npos) {
				PH_LOG_WARNING("It was not possible to find tileset for " + std::to_string(globalTileId));
				continue;
			}
			const unsigned tileId = globalTileId - tilesets.firstGlobalTileIds[tilesetIndex];
			sf::Vector2u tileRectPosition =
				Math::getTwoDimensionalPositionFromOneDimensionalArrayIndex(tileId, tilesets.columnsCounts[tilesetIndex]);
			tileRectPosition.x *= info.tileSize.x;
			tileRectPosition.y *= info.tileSize.y;

			sf::Vector2f position(Math::getTwoDimensionalPositionFromOneDimensionalArrayIndex(tileIndexInMap, info.mapSize.x));
			position.x *= info.tileSize.x;
			position.y *= info.tileSize.y;

			auto tileEntity = mTemplates->createCopy("Tile", *mGameRegistry);
			auto& bodyRect = mGameRegistry->get<component::BodyRect>(tileEntity);
			auto& textureRect = mGameRegistry->get<component::TextureRect>(tileEntity);
			auto& renderQuad = mGameRegistry->get<component::RenderQuad>(tileEntity);

			bodyRect.rect.left = position.x;
			bodyRect.rect.top = position.y;
			bodyRect.rect.width = static_cast<float>(info.tileSize.x);
			bodyRect.rect.height = static_cast<float>(info.tileSize.y);

			textureRect.rect.left = tileRectPosition.x;
			textureRect.rect.top = tileRectPosition.y;
			textureRect.rect.width = info.tileSize.x;
			textureRect.rect.height = info.tileSize.y;

			renderQuad.texture = &texture;
			renderQuad.z = z;

			const std::size_t tilesDataIndex = findTilesIndex(tilesets.firstGlobalTileIds[tilesetIndex], tilesets.tilesData);
			if (tilesDataIndex == std::string::npos)
				continue;
			//loadCollisionBodies(tileId, tilesets.tilesData[tilesDataIndex], position);
		}
	}
}

bool XmlMapParser::hasTile(unsigned globalTileId) const
{
	return globalTileId != 0;
}

std::size_t XmlMapParser::findTilesetIndex(const unsigned globalTileId, const TilesetsData& tilesets) const
{
	for (std::size_t i = 0; i < tilesets.firstGlobalTileIds.size(); ++i) {
		const unsigned firstGlobalTileId = tilesets.firstGlobalTileIds[i];
		const unsigned lastGlobalTileId = firstGlobalTileId + tilesets.tileCounts[i] - 1;
		if (globalTileId >= firstGlobalTileId && globalTileId <= lastGlobalTileId)
			return i;
	}
	return std::string::npos;
}

std::size_t XmlMapParser::findTilesIndex(const unsigned firstGlobalTileId, const std::vector<TilesData>& tilesData) const
{
	for (std::size_t i = 0; i < tilesData.size(); ++i)
		if (firstGlobalTileId == tilesData[i].firstGlobalTileId)
			return i;
	return std::string::npos;
}

void XmlMapParser::loadCollisionBodies(const unsigned tileId, const TilesData& tilesData, const sf::Vector2f position)
{
	for (std::size_t i = 0; i < tilesData.ids.size(); ++i) {
		if (tileId == tilesData.ids[i]) {
			sf::FloatRect bounds = tilesData.bounds[i];
			bounds.left += position.x;
			bounds.top += position.y;
			//mGameData->getPhysicsEngine().createStaticBodyAndGetTheReference(bounds);   // TUTAJ
			//mGameData->getAIManager().registerObstacle({ bounds.left, bounds.top });   // TUTAJ
		}
	}
}

void XmlMapParser::createMapBorders(const GeneralMapInfo& mapInfo)
{
	// TUTAJ

	auto mapWidth = static_cast<float>(mapInfo.mapSize.x * mapInfo.tileSize.x);
	auto mapHeight = static_cast<float>(mapInfo.mapSize.y * mapInfo.tileSize.y);

	const sf::Vector2f size(sf::Vector2u(mapInfo.tileSize.x, mapInfo.tileSize.y));

	//auto& physics = mGameData->getPhysicsEngine();

	for (int x = -1; x < static_cast<int>(mapInfo.mapSize.x + 1); ++x)
	{
		// top border
		sf::Vector2f positionTop(x * size.x, -size.y);
		//physics.createStaticBodyAndGetTheReference({ positionTop, size });
		// bottom border
		sf::Vector2f positionBottom(x * size.x, mapHeight);
		//physics.createStaticBodyAndGetTheReference({ positionBottom, size });
	}

	for (int y = 0; y < static_cast<int>(mapInfo.mapSize.y); ++y)
	{
		// left border
		sf::Vector2f positionLeft(-size.x, y * size.y);
		//physics.createStaticBodyAndGetTheReference({ positionLeft, size });
		// right border
		sf::Vector2f positionRight(mapWidth, y * size.y);
		//physics.createStaticBodyAndGetTheReference({ positionRight, size });
	}
}

}
