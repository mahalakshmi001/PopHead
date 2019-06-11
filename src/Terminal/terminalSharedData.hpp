#ifndef POPHEAD_TERMINAL_TERMINALSHAREDDATA_H_
#define POPHEAD_TERMINAL_TERMINALSHAREDDATA_H_

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

namespace ph {
namespace Terminal {

struct TerminalData
{
	sf::Text mText;
	std::string mContent;
	bool mIsVisible = false;
};

using TerminalSharedData = std::shared_ptr<TerminalData>;

}}

#endif // !POPHEAD_TERMINAL_TERMINALSHAREDDATA_H_

