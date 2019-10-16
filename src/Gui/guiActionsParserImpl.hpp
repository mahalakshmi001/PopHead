#pragma once

#include "guiActionsParser.hpp"

namespace ph {


class GuiActionsParserImpl : public GuiActionsParser
{
public:
	std::function<void(Widget*)> getGuiAction(GameData& mGameData, const std::string& actionStr) const override;

private:
	std::pair<std::string, std::string> getSplitAction(const std::string& actionStr) const;
};

}
