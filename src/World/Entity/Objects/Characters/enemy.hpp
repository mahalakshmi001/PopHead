#ifndef POPHEAD_WORLD_ENTITY_OBJECTS_CHARACTERS_ENEMY_H_
#define POPHEAD_WORLD_ENTITY_OBJECTS_CHARACTERS_ENEMY_H_

#include "World/Entity/Objects/character.hpp"
#include <set>

namespace ph {
namespace World {
namespace Entity {


class Enemy : public Character
{
public:
	Enemy(ph::Base::GameData* gameData, std::string name, ph::World::Animation animation,
		unsigned int movementSpeed, unsigned int HP, unsigned int maxHP, sf::FloatRect posAndSize, float mass);

	///auto getDamageWeaknesses() const -> const std::set<DamageWeakness>;

protected:
	///std::set<DamageWeakness> mDamageWeaknesses;
};


}}}

#endif // !POPHEAD_WORLD_ENTITY_OBJECTS_CHARACTERS_ENEMY_H_
