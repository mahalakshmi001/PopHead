#include "GameObjects/DrawableGameObjects/Characters/Npcs/gateGuard.hpp"
#include "GameObjects/DrawableGameObjects/gate.hpp"
#include "GameObjects/NonDrawableGameObjects/activateGateAreas.hpp"
#include "Utilities/math.hpp"

namespace ph {

GateGuard::GateGuard(GameData* const gameData)
	:Npc(gameData, "gateGuard")
	,mOpened(false)
{
}

void GateGuard::updateCurrent(const sf::Time delta)
{
	auto* playerGameObject = mRoot->getChild("LAYER_standingObjects")->getChild("player");
	if (playerGameObject == nullptr)
		return;
	auto * player = dynamic_cast<Character*>(playerGameObject);
	auto* invisibleObjects = mRoot->getChild("LAYER_invisibleObjects");

	if (!mOpened)
	{
		auto* openGateArea = dynamic_cast<OpenGateArea*>(invisibleObjects->getChild("openGateArea"));
		if(openGateArea->getActivated())
			openGate();
	}

	else
	{
		auto* closeGateArea = dynamic_cast<CloseGateArea*>(invisibleObjects->getChild("closeGateArea"));
		if (closeGateArea->getActivated())
			closeGate();
	}
}

void GateGuard::openGate()
{
	mOpened = true;
	auto* lyingObjects = mRoot->getChild("LAYER_lyingObjects");
	auto* gate = dynamic_cast<Gate*>(lyingObjects->getChild("gate"));
	gate->open();
}


void GateGuard::closeGate()
{
	auto* lyingObjects = mRoot->getChild("LAYER_lyingObjects");
	auto* gate = dynamic_cast<Gate*>(lyingObjects->getChild("gate"));
	gate->close();
}

}