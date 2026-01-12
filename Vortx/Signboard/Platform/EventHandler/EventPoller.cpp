#include "EventPoll.h"

#include "Signboard/Core/Time/Time.h"

#include <GLFW/glfw3.h>
#include <chrono>

EventPoller::EventPoller(GLFWwindow* window) 
	: window(window)
{
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, keyCallBack);
	glfwSetScrollCallback(window, scrollCallBack);
	glfwSetMouseButtonCallback(window, mouseButtonCallBack);
	glfwSetCursorPosCallback(window, cursorMoveCallBack);
}

void EventPoller::poll() {
	glfwPollEvents();
}

void EventPoller::fetchEvents(std::vector<InputEvent>& out) {
	out.clear();
	out.swap(toResolveList);
}

void EventPoller::keyCallBack(GLFWwindow* window, int key, int scanCode, int action, int mods) {
	auto* self = static_cast<EventPoller*>(glfwGetWindowUserPointer(window));

	InputEvent e{};
	e.type = InputEvent::Type::Key;
	e.key = { key, action, mods };
	e.timestamp = Time::now();

	self->toResolveList.push_back(e);
}

void EventPoller::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
	auto* self = static_cast<EventPoller*>(glfwGetWindowUserPointer(window));

	InputEvent e{};
	e.type = InputEvent::Type::Scroll;
	e.scroll = { xoffset, yoffset };
	e.timestamp = Time::now();

	self->toResolveList.push_back(e);
}

void EventPoller::mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
	auto* self = static_cast<EventPoller*>(glfwGetWindowUserPointer(window));

	InputEvent e{};
	e.type = InputEvent::Type::MouseButton;
	e.mouseButton = { button, action, mods };
	e.timestamp = Time::now();

	self->toResolveList.push_back(e);
}

void EventPoller::cursorMoveCallBack(GLFWwindow* window, double xpos, double ypos) {
	auto* self = static_cast<EventPoller*>(glfwGetWindowUserPointer(window));
	InputEvent e{};
	e.type = InputEvent::Type::CursorMove;
	e.cursorMove = { xpos, ypos };
	e.timestamp = Time::now();

	self->toResolveList.push_back(e);
}