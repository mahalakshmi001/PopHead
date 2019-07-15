#pragma once

#include "cast.hpp"
#include <string>
#include <vector>

namespace ph {

class Xml 
{
public:
	void loadFromFile(std::string filePath);

	Xml getChild(std::string name) const;

	std::vector<Xml> getChildren(std::string name) const;

	bool hasAttribute(std::string name) const;

	Xml getAttribute(std::string name) const;

	std::string toString() const;

	bool toBool() const 
	{ const std::string str = toString(); return Cast::toBool(str); }

	int toInt() const 
	{ const std::string str = toString(); return std::stoi(str); }

	unsigned toUnsigned() const 
	{ const std::string str = toString(); return Cast::toUnsigned(str); }

	float toFloat() const 
	{ const std::string str = toString(); return std::stof(str); }

private:
	bool isSelfClosingTag(std::size_t openingTagEndPosition) const
	{ return mContent[openingTagEndPosition - 1] == '/'; }

	bool isClosingTag(std::size_t tagNamePosition) const 
	{ return mContent[tagNamePosition - 2] == '<' && mContent[tagNamePosition - 1] == '/'; }

	bool isEmptyAttributeValue(std::size_t onePositionAfterAttributeValueOpeningQuote) const
	{ return mContent[onePositionAfterAttributeValueOpeningQuote] == '\"'; }

	std::size_t findEndOfTagAttributes(std::size_t offset = 0) const
	{ return mContent.find('>', offset); }

	inline static const std::string whitespaceCharacters = " \n\t\v\f\r";
	std::string mContent;
};

}
