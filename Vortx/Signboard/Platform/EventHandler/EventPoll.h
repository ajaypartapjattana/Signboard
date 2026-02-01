#pragma once

#include <vector>

struct InputEvent {
	enum class Type {
		Key,
		MouseButton,
		CursorMove,
		Scroll
	} type;
	union {
		struct {
			int key;
			int action;
			int mods;
		} key;

		struct {
			int button;
			int action;
			int mods;
		} mouseButton;

		struct {
			double x, y;
		} cursorMove;

		struct {
			double xOffset, yOffset;
		} scroll;
	};
	float timestamp;
};

struct GLFWwindow;

class EventPoller {
public:
	EventPoller(GLFWwindow* window);

	void poll();
	void fetchEvents(std::vector<InputEvent>& out);

private:
	GLFWwindow* window;

	std::vector<InputEvent> toResolveList;

	static void keyCallBack(GLFWwindow*, int, int, int, int);
	static void scrollCallBack(GLFWwindow*, double, double);
	static void mouseButtonCallBack(GLFWwindow*, int, int, int);
	static void cursorMoveCallBack(GLFWwindow*, double, double);

};