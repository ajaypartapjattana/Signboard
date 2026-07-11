#include "convStr.h"

#include <charconv>
#include <stdexcept>

int convStr::toInt(const std::string& s) {
	int value{};
	auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

	if (ec != std::errc() || ptr != s.data() + s.size())
		throw std::runtime_error("Invalid integer: " + s);

	return value;
}

float convStr::toFloat(const std::string& s) {
	float value{};
	auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

	if (ec != std::errc() || ptr != s.data() + s.size())
		throw std::runtime_error("Invalid float: " + s);

	return value;
}