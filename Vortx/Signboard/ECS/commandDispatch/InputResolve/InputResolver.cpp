#include "InputResolver.h"

#include <GLFW/glfw3.h>

void InputResolver::resolveInputs() {
	m_mousePosPrev = m_mousePos;
	m_scrollDelta = { 0.0f, 0.0f };

	m_keysPrev = m_keys;
	m_mouseButtonsPrev = m_mouseButtons;

	for (const InputEvent& e : m_events) {
		switch (e.type) {
		case InputEvent::Type::CursorMove:
			m_mousePos = {
				(float)e.cursorMove.x, 
				(float)e.cursorMove.y 
			};
			break;

		case InputEvent::Type::Scroll:
			m_scrollDelta += glm::vec2{
				(float)e.scroll.xOffset, 
				(float)e.scroll.yOffset
			};
			break;

		case InputEvent::Type::Key:
			if (e.key.action == GLFW_PRESS)
				m_keys.insert(e.key.key);
			else if (e.key.action == GLFW_RELEASE)
				m_keys.erase(e.key.key);
			break;

		case InputEvent::Type::MouseButton:
			if (e.mouseButton.action == GLFW_PRESS)
				m_mouseButtons.insert(e.mouseButton.button);
			else if (e.mouseButton.action == GLFW_RELEASE)
				m_mouseButtons.erase(e.mouseButton.button);
			break;
		}
	}
}

bool InputResolver::isKeyPressed(int key) const {
	return m_keys.find(key) != m_keys.end() && m_keysPrev.find(key) == m_keysPrev.end();
}

bool InputResolver::isKeyHeld(int key) const {
	return m_keys.find(key) != m_keys.end();
}

bool InputResolver::isKeyReleased(int key) const {
	return m_keys.find(key) == m_keys.end() && m_keysPrev.find(key) != m_keysPrev.end();
}

bool InputResolver::isMousePressed(int button) const {
	return m_mouseButtons.find(button) != m_mouseButtons.end() && m_mouseButtonsPrev.find(button) != m_mouseButtonsPrev.end();
}

bool InputResolver::isMouseHeld(int button) const {
	return m_mouseButtons.find(button) != m_mouseButtons.end();
}

bool InputResolver::isMouseReleased(int button) const {
	return m_mouseButtons.find(button) == m_mouseButtons.end() && m_mouseButtonsPrev.find(button) != m_mouseButtonsPrev.end();
}

glm::vec2 InputResolver::MousePosition() const {
	return m_mousePos;
}

glm::vec2 InputResolver::MouseDelta() const {
	glm::vec2 delta = m_mousePos - m_mousePosPrev;
	return delta;
}

glm::vec2 InputResolver::ScrollDelta() const {
	glm::vec2 delta = m_scrollDelta;
	return delta;
}