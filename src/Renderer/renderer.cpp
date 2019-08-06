#include "Renderer/renderer.hpp"
#include "GameObjects/drawableGameObject.hpp"
#include "Logs/logs.hpp"
#include "Utilities/math.hpp"
#include "gameData.hpp"

namespace ph {

Renderer::Renderer(sf::RenderTarget& renderTarget)
	:mRenderTarget(renderTarget)
	,mCamera{ sf::Vector2f{0,0}, sf::Vector2f{16*40, 16*30} }
	,mViewports{ { FullScreenViewport, { 0.f, 0.f, 1.f, 1.f } } }
	,mLayers{  { LayerID::floorEntities, Layer() },
				{ LayerID::staticEntities, Layer() },
				{ LayerID::kinematicEntities, Layer() },
				{ LayerID::airEntities, Layer() },
				{ LayerID::gui, Layer() },
				{ LayerID::cmd, Layer() } }
	,mDebugRenderingMode(false)
{
	mCamera.setViewport(mViewports.at(FullScreenViewport));
}

Renderer::~Renderer()
{
}

void Renderer::update(sf::Time delta)
{
	mCamera.update(delta);
	setPositionOfStaticObjectsToCamera();
}

void Renderer::draw() const
{
	mCamera.applyTo(mRenderTarget);
	mRenderTarget.clear();

	sf::FloatRect properCameraBounds = getProperCameraBounds();
	mGameData->getMap().draw(mRenderTarget, sf::RenderStates::Default, properCameraBounds);
	drawSceneLayers(properCameraBounds);
	mRenderTarget.draw(mGameData->getPhysicsEngine().getCollisionDebugManager());
	mRenderTarget.draw(mGameData->getEfficiencyRegister().getDisplayer());
	mRenderTarget.draw(mGameData->getTerminal().getImage());
}

void Renderer::drawSceneLayers(sf::FloatRect properCameraBounds) const
{
	for(const auto& layer : mLayers)
		for(const auto& drawableGameObject : layer.second) {
			auto objectPosition = drawableGameObject->getPosition();
			sf::FloatRect objectBounds(objectPosition.x - 100, objectPosition.y - 100, 200, 200);
			if(Math::areTheyOverlapping(properCameraBounds, objectBounds)) {
				mRenderTarget.draw(*drawableGameObject);
				mGameData->getEfficiencyRegister().registerRenderCall();
			}
		}
}

sf::FloatRect Renderer::getProperCameraBounds() const
{
	if(mDebugRenderingMode) {
		sf::Vector2f cameraCommonSize(640, 480);
		const sf::Vector2f cameraCommonLeftCornerPosition(
			mCamera.getCenter().x - cameraCommonSize.x / 2,
			mCamera.getCenter().y - cameraCommonSize.x / 2
		);
		return sf::FloatRect(cameraCommonLeftCornerPosition.x, cameraCommonLeftCornerPosition.y,
			cameraCommonSize.x, cameraCommonSize.y);
	}
	else
		return mCamera.getBounds();
}

void Renderer::addObject(DrawableGameObject* const object)
{
	mLayers[object->getLayerID()].addObject(object);
	PH_LOG_INFO("Object \"" + object->getName() + "\" was added to " + getLayerName(object->getLayerID()) + " layer.");
}

void Renderer::addObject(DrawableGameObject* const object, LayerID layerID)
{
	mLayers[layerID].addObject(object);
	PH_LOG_INFO("Object \"" + object->getName() + "\" was added to " + getLayerName(layerID) + " layer.");
}

void Renderer::removeDrawableGameObject(std::string name, LayerID layerId)
{
	mLayers[layerId].removeObject(name);
	PH_LOG_INFO("Object \"" + name + "\" was removed from " + getLayerName(layerId) + " layer.");
}

void Renderer::removeDrawableGameObject(const DrawableGameObject* const object)
{
	PH_LOG_INFO("Object \"" + object->getName() + "\" was removed from " + getLayerName(object->getLayerID()) + " layer.");
	mLayers[object->getLayerID()].removeObject(object);
}

void Renderer::removeAllObjectsFromLayer(LayerID layerID)
{
	mLayers[layerID].clear();
	PH_LOG_INFO("All objects were removed from " + getLayerName(layerID) + " layer.");
}

void Renderer::clear() noexcept
{
	for(auto& layer : mLayers)
		layer.second.clear();
}

void Renderer::setPositionOfStaticObjectsToCamera()
{
	const sf::Vector2f movementFromLastFrame = mCamera.getCameraMoveFromLastFrame();
	for(const auto& guiObject : mLayers[LayerID::gui]) {
		guiObject->move(movementFromLastFrame);
	}
	mGameData->getTerminal().getImage().move(movementFromLastFrame);
	mGameData->getEfficiencyRegister().getDisplayer().move(movementFromLastFrame);
}

std::string Renderer::getLayerName(LayerID layerID) const
{
	switch(layerID)
	{
	case LayerID::floorEntities:     return "floorEntities";
	case LayerID::staticEntities:    return "staticEntities";
	case LayerID::kinematicEntities: return "kinematicEntities";
	case LayerID::airEntities:       return "airEntities";
	case LayerID::collisionDebug:    return "collisionDebug";
	case LayerID::gui:               return "gui";
	case LayerID::cmd:               return "cmd";
	default:
		PH_UNEXPECTED_SITUATION("Every object should be bound to the certain layer!");
	}
}

}