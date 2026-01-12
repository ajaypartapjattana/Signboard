#include "Time.h"

#include <chrono>

float Time::now() {
	static const auto start = std::chrono::steady_clock::now();
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count();
}