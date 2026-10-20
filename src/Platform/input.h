#pragma once

#include <initializer_list>

enum Action : uint8_t {
	ACTION_NONE,
	ACTION_PRESS,
	ACTION_REPEAT,
	ACTION_RELEASE
};

enum Key : uint16_t {
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_ESCAPE,
	KEY_MINUS,
	KEY_EQUALS,
	KEY_LEFT_BRACKET,
	KEY_RIGHT_BRACKET,
	KEY_BACKSLASH,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_GRAVE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_NON_US_BACKSLASH,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_ENTER,
	KEY_SPACE,
	KEY_ARROW_LEFT,
	KEY_ARROW_RIGHT,
	KEY_ARROW_UP,
	KEY_ARROW_DOWN,
	KEY_SHIFT_LEFT,
	KEY_SHIFT_RIGHT,
	KEY_CTRL_LEFT,
	KEY_CTRL_RIGHT,
	KEY_ALT_LEFT,
	KEY_ALT_RIGHT,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_SCROLL_LOCK,
	KEY_NUMPAD_0,
	KEY_NUMPAD_1,
	KEY_NUMPAD_2,
	KEY_NUMPAD_3,
	KEY_NUMPAD_4,
	KEY_NUMPAD_5,
	KEY_NUMPAD_6,
	KEY_NUMPAD_7,
	KEY_NUMPAD_8,
	KEY_NUMPAD_9,
	KEY_NUMPAD_ADD,
	KEY_NUMPAD_SUBTRACT,
	KEY_NUMPAD_MULTIPLY,
	KEY_NUMPAD_DIVIDE,
	KEY_NUMPAD_ENTER,
	KEY_NUMPAD_DECIMAL,
	KEY_PRINT_SCREEN,
	KEY_PAUSE,

	KEY_IMP
};

enum Button : uint8_t {
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_MIDDLE,
	BUTTON_4,
	BUTTON_5,
	BUTTON_TOUCH,

	BUTTON_IMP
};

struct IVEC2D {
	int32_t x;
	int32_t y;
};

class WindowInput {
	static constexpr size_t KEY_PAGE_COUNT = (KEY_IMP + 63u) >> 6;
	struct {
		uint64_t repeat[KEY_PAGE_COUNT];
		uint64_t press[KEY_PAGE_COUNT];
		uint64_t release[KEY_PAGE_COUNT];
	} keyField{};
	struct {
		uint32_t repeat;
		uint32_t press;
		uint32_t release;
	} buttonField{};
	struct {
		int32_t scroll;
		int32_t scrollDelta;
		int32_t position[2];
		int32_t positionDelta[2];
	} pointr{};

	static constexpr uint32_t page(Key key) noexcept {
		return key >> 6;
	}

	static constexpr uint64_t bit(Key key) noexcept {
		return (uint64_t)1u << (key & 0x3f);
	}

	static constexpr uint32_t bit(Button button) noexcept {
		return 1u << button;
	}

public:
	void press(Key key) noexcept {
		keyField.press[page(key)] |= bit(key);
	}

	void press(Button button) noexcept {
		buttonField.press |= bit(button);
	}

	void repeat(Key key) noexcept {
		keyField.repeat[page(key)] |= bit(key);
	}

	void repeat(Button button) noexcept {
		buttonField.repeat |= bit(button);
	}

	void release(Key key) noexcept {
		keyField.release[page(key)] |= bit(key);
	}

	void release(Button button) noexcept {
		buttonField.release |= bit(button);
	}

	void level(int32_t x) noexcept {
		pointr.scroll = x;
	}

	void point(int32_t x, int32_t y) noexcept {
		pointr.position[0] = x;
		pointr.position[1] = y;
	}

	void scroll(int32_t x) noexcept {
		pointr.scrollDelta += x;
	}

	void drag(int32_t x, int32_t y) noexcept {
		pointr.positionDelta[0] += x;
		pointr.positionDelta[1] += y;
	}

	void lapse() noexcept {
		pointr.scroll += pointr.scrollDelta;
		pointr.position[0] += pointr.positionDelta[0];
		pointr.position[1] += pointr.positionDelta[1];
	}

	void flush() noexcept {
		keyField.repeat[0] |= keyField.press[0];
		keyField.press[0] ^= keyField.press[0];
		keyField.repeat[1] |= keyField.press[1];
		keyField.press[1] ^= keyField.press[1];
		keyField.repeat[0] &= ~keyField.release[0];
		keyField.release[0] ^= keyField.release[0];
		keyField.repeat[1] &= ~keyField.release[1];
		keyField.release[1] ^= keyField.release[1];
		buttonField.repeat |= buttonField.press;
		buttonField.press ^= buttonField.press;
		buttonField.repeat &= ~buttonField.release;
		buttonField.release ^= buttonField.release;
		pointr.scrollDelta ^= pointr.scrollDelta;
		pointr.positionDelta[0] ^= pointr.positionDelta[0];
		pointr.positionDelta[1] ^= pointr.positionDelta[1];
	}

	bool testDown() const noexcept {
		if (keyField.repeat[0] || keyField.repeat[1])
			return true;

		if (buttonField.repeat)
			return true;

		return false;
	}

	bool isDown(Key key) const noexcept {
		uint32_t keyPage = page(key);
		uint64_t keyBit = bit(key);
		return (keyField.repeat[keyPage] & keyBit) || (keyField.press[keyPage] & keyBit);
	}

	bool isDown(Button button) const noexcept {
		uint64_t buttonBit = bit(button);
		return (buttonField.repeat & buttonBit) || (buttonField.press & buttonBit);
	}

	bool isPressed(Key key) const noexcept {
		return keyField.press[page(key)] & bit(key);
	}

	bool isPressed(Button button) const noexcept {
		return buttonField.press & bit(button);
	}

	bool isHeld(Key key) const noexcept {
		return keyField.repeat[page(key)] & bit(key);
	}

	bool isHeld(Button button) const noexcept {
		return buttonField.repeat & bit(button);
	}

	bool isReleased(Key key) const noexcept {
		return keyField.release[page(key)] & bit(key);
	}

	bool isReleased(Button button) const noexcept {
		return buttonField.release & bit(button);
	}

	IVEC2D pointer() const noexcept {
		return { pointr.position[0], pointr.position[1] };
	}

	IVEC2D pointerDelta() const noexcept {
		return { pointr.positionDelta[0], pointr.positionDelta[1] };
	}

	int32_t scroll() const noexcept {
		return pointr.scroll;
	}

	int32_t scrollDelta() const noexcept {
		return pointr.scrollDelta;
	}
};

