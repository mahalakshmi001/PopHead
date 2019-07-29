#include "gameObject.hpp"
#include "Logs/logs.hpp"

#include <stdexcept>

namespace ph {

GameObject::GameObject(const std::string& name)
	:mName(name)
	,mParent(nullptr)
{
}

void GameObject::input()
{
	for(auto& child : mChildren)
		child->input();
}

void GameObject::update(sf::Time delta)
{
	for(auto& child : mChildren)
		child->update(delta);
}

std::string GameObject::getUniqueName(std::string& childName) const
{
	for (const auto& child : mChildren)
		if (child->getName() == childName)
			while(child->getName() == childName)
				correctChildName(childName);
	return childName;
}

void GameObject::correctChildName(std::string& childName) const
{
	if (childName.find('_') != std::string::npos)
		incrementNumberInChildName(childName);
	else
		childName += "_2";
}

void GameObject::incrementNumberInChildName(std::string& childName) const
{
	std::size_t begin = childName.find('_');
	childName.replace(begin + 1, std::string::npos,
		std::to_string(std::stoi(childName.substr(begin + 1))+1));
}

void GameObject::addChild(std::unique_ptr<GameObject> newChild)
{
	const std::string nameOfNewChild = getUniqueName(newChild->mName);
	newChild->mName = nameOfNewChild;
    newChild->mParent = this;
    mChildren.emplace_back(std::move(newChild));
	PH_LOG(LogLevel::Info, "GameObject \"" + nameOfNewChild + "\" was added as child of the \"" + mName + "\"");
}

void GameObject::removeChild(const std::string& name)
{
	for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
		if ((*it)->getName() == name) {
            mChildren.erase(it);
			break;
		}

	PH_LOG(LogLevel::Info, "GameObject \"" + name + "\" was removed. It was a child of the \"" + mName + "\"");
}

void GameObject::removeChild(GameObject* childToRemove)
{
	PH_LOG(LogLevel::Info, "GameObject \"" + childToRemove->getName() + "\" was removed. It was a child of the \"" + mName + "\"");

	for(auto it = mChildren.begin(); it != mChildren.end(); ++it)
		if(it->get() == childToRemove) {
			mChildren.erase(it);
			break;
		}
}

auto GameObject::getChild(const std::string& name) const -> GameObject&
{
    for(auto const &child : mChildren)
        if(child->getName() == name)
            return *(child.get());
	throw std::runtime_error("Child was not found!");
}

}