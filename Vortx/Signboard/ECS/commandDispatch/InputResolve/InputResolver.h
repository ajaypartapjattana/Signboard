#pragma once

#include "Signboard/Platform/EventHandler/EventPoll.h"

#include <glm/glm.hpp>
#include <unordered_set>
#include <vector>
#include <cstdint>

class InputResolver {
public:
	void resolveInputs();

	bool isKeyPressed(int key) const;
	bool isKeyReleased(int key) const;
	bool isKeyHeld(int key) const;

	bool isMousePressed(int button) const;
	bool isMouseReleased(int button) const;
	bool isMouseHeld(int button) const;

	glm::vec2 MousePosition() const;
	glm::vec2 MouseDelta() const;
	glm::vec2 ScrollDelta() const;

	std::vector<InputEvent>& getFrameEvents() { return m_events; }

private:
	std::vector<InputEvent> m_events;

	std::unordered_set<int> m_keys;
	std::unordered_set<int> m_keysPrev;

	std::unordered_set<int> m_mouseButtons;
	std::unordered_set<int> m_mouseButtonsPrev;

	glm::vec2 m_mousePos{ 0.0f };
	glm::vec2 m_mousePosPrev{ 0.0f };

	glm::vec2 m_scrollDelta{ 0.0f };
};
