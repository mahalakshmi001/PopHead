#include "cast.hpp"
#include "Logs/logs.hpp"
#include <unordered_map>

namespace ph::Cast {

unsigned toUnsigned(const std::string& str)
{
	const unsigned long ulResult = std::stoul(str);
	const unsigned result = ulResult;
	if (result != ulResult)
		PH_EXCEPTION("unsigned type is too small to contain conversion result");
	return result;
}

std::string toString(const sf::Vector2f& vec)
{
	std::string xVal = std::to_string(vec.x);
	std::string yVal = std::to_string(vec.y);
	return "x:" + xVal + " y:" + yVal;
}

bool toBool(const std::string& str)
{
	if (str == "true" || str == "1")
		return true;
	else if (str == "false" || str == "0")
		return false;
	else
		PH_EXCEPTION("Cast to bool failed!");
}

Vector4f toNormalizedColorVector4f(const sf::Color& color)
{
	return Vector4f({
		static_cast<float>(color.r) / 255.f, static_cast<float>(color.g) / 255.f,
		static_cast<float>(color.b) / 255.f, static_cast<float>(color.a) / 255.f
	});
}

}
