#pragma once

#include "Signboard/Platform/platform.h"

#include <glm/glm.hpp>
#include <bitset>
#include <vector>
#include <cstdint>

class InputResolver {
public:
	InputResolver() noexcept;

	void resolveInputs(platform::primitive::eventStateInputsAccess&& m_inputsView);

	bool isKeyPressed(int key) const;
	bool isKeyReleased(int key) const;
	bool isKeyHeld(int key) const;

	bool isMousePressed(int button) const;
	bool isMouseReleased(int button) const;
	bool isMouseHeld(int button) const;

	glm::vec2 MousePosition() const;
	glm::vec2 MouseDelta() const;
	glm::vec2 ScrollDelta() const;

private:
	static constexpr size_t MAX_KEYS = 512;
	static constexpr size_t MAX_MOUSE = 16;

	std::bitset<MAX_KEYS> m_keys;
	std::bitset<MAX_KEYS> m_keysPrev;

	std::bitset<MAX_MOUSE> m_mouseButtons;
	std::bitset<MAX_MOUSE> m_mouseButtonsPrev;

	glm::vec2 m_mousePos{ 0.0f };
	glm::vec2 m_mousePosPrev{ 0.0f };

	glm::vec2 m_scrollDelta{ 0.0f };
};
