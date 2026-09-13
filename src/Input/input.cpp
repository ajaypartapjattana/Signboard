#include <stdlib.h>
#include <cstring>
#include <cassert>

#include "input.h"

static inline InputKeyModifierBit getKeyModifierBit(const InputKey _Key) noexcept {
	switch (_Key) {
	case INPUT_KEY_SHIFT_LEFT: return INPUT_KEY_MODIFIER_SHIFT;
	case INPUT_KEY_SHIFT_RIGHT: return INPUT_KEY_MODIFIER_SHIFT;
	case INPUT_KEY_CTRL_LEFT: return INPUT_KEY_MODIFIER_CTRL;
	case INPUT_KEY_CTRL_RIGHT: return INPUT_KEY_MODIFIER_CTRL;
	case INPUT_KEY_ALT_LEFT : return INPUT_KEY_MODIFIER_ALT;
	case INPUT_KEY_ALT_RIGHT : return INPUT_KEY_MODIFIER_ALT;
	default: return (InputKeyModifierBit)0u;
	}
}

#if defined(PLATFORM_WINDOWS)

#elif defined(PLATFORM_LINUX)
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <libudev.h>

#include <core/Memory/memory.h>

static inline InputKeyState translateKeyStateImp(const signed int _Value) noexcept {
	switch (_Value) {
	case 0: return INPUT_KEY_STATE_RELEASED;
	case 1: return INPUT_KEY_STATE_PRESSED;
	case 2: return INPUT_KEY_STATE_REPEAT;

	default: return INPUT_KEY_STATE_UNDEFINED;
	}
}

static inline InputButtons translateMouseButtonImp(const uint16_t _Code) noexcept {
	switch (_Code) {
	case BTN_LEFT: return INPUT_BUTTON_BIT_LEFT;
	case BTN_RIGHT: return INPUT_BUTTON_BIT_RIGHT;
	case BTN_MIDDLE: return INPUT_BUTTON_BIT_MIDDLE;
	default: return 0u;
	}
}

static InputKey translateKeyImp(const unsigned short _Code) noexcept {
	switch (_Code) {
	case KEY_A: return INPUT_KEY_A;
	case KEY_B: return INPUT_KEY_B;
	case KEY_C: return INPUT_KEY_C;
	case KEY_D: return INPUT_KEY_D;
	case KEY_E: return INPUT_KEY_E;
	case KEY_F: return INPUT_KEY_F;
	case KEY_G: return INPUT_KEY_G;
	case KEY_H: return INPUT_KEY_H;
	case KEY_I: return INPUT_KEY_I;
	case KEY_J: return INPUT_KEY_J;
	case KEY_K: return INPUT_KEY_K;
	case KEY_L: return INPUT_KEY_L;
	case KEY_M: return INPUT_KEY_M;
	case KEY_N: return INPUT_KEY_N;
	case KEY_O: return INPUT_KEY_O;
	case KEY_P: return INPUT_KEY_P;
	case KEY_Q: return INPUT_KEY_Q;
	case KEY_R: return INPUT_KEY_R;
	case KEY_S: return INPUT_KEY_S;
	case KEY_T: return INPUT_KEY_T;
	case KEY_U: return INPUT_KEY_U;
	case KEY_V: return INPUT_KEY_V;
	case KEY_W: return INPUT_KEY_W;
	case KEY_X: return INPUT_KEY_X;
	case KEY_Y: return INPUT_KEY_Y;
	case KEY_Z: return INPUT_KEY_Z;
	case KEY_0: return INPUT_KEY_0;
	case KEY_1: return INPUT_KEY_1;
	case KEY_2: return INPUT_KEY_2;
	case KEY_3: return INPUT_KEY_3;
	case KEY_4: return INPUT_KEY_4;
	case KEY_5: return INPUT_KEY_5;
	case KEY_6: return INPUT_KEY_6;
	case KEY_7: return INPUT_KEY_7;
	case KEY_8: return INPUT_KEY_8;
	case KEY_9: return INPUT_KEY_9;
	case KEY_ESC: return INPUT_KEY_ESCAPE;
	case KEY_INSERT: return INPUT_KEY_INSERT;
	case KEY_DELETE: return INPUT_KEY_DELETE;
	case KEY_HOME: return INPUT_KEY_HOME;
	case KEY_END: return INPUT_KEY_END;
	case KEY_PAGEUP: return INPUT_KEY_PAGE_UP;
	case KEY_PAGEDOWN: return INPUT_KEY_PAGE_DOWN;
	case KEY_BACKSPACE: return INPUT_KEY_BACKSPACE;
	case KEY_TAB: return INPUT_KEY_TAB;
	case KEY_ENTER: return INPUT_KEY_ENTER;
	case KEY_SPACE: return INPUT_KEY_SPACE;
	case KEY_LEFT: return INPUT_KEY_ARROW_LEFT;
	case KEY_RIGHT: return INPUT_KEY_ARROW_RIGHT;
	case KEY_UP: return INPUT_KEY_ARROW_UP;
	case KEY_DOWN: return INPUT_KEY_ARROW_DOWN;
	case KEY_LEFTSHIFT: return INPUT_KEY_SHIFT_LEFT;
	case KEY_RIGHTSHIFT: return INPUT_KEY_SHIFT_RIGHT;
	case KEY_LEFTCTRL: return INPUT_KEY_CTRL_LEFT;
	case KEY_RIGHTCTRL: return INPUT_KEY_CTRL_RIGHT;
	case KEY_LEFTALT: return INPUT_KEY_ALT_LEFT;
	case KEY_RIGHTALT: return INPUT_KEY_ALT_RIGHT;
	case KEY_CAPSLOCK: return INPUT_KEY_CAPSLOCK;
	case KEY_NUMLOCK: return INPUT_KEY_NUMLOCK;
	case KEY_SCROLLLOCK: return INPUT_KEY_SCROLL_LOCK;
	case KEY_F1: return INPUT_KEY_F1;
	case KEY_F2: return INPUT_KEY_F2;
	case KEY_F3: return INPUT_KEY_F3;
	case KEY_F4: return INPUT_KEY_F4;
	case KEY_F5: return INPUT_KEY_F5;
	case KEY_F6: return INPUT_KEY_F6;
	case KEY_F7: return INPUT_KEY_F7;
	case KEY_F8: return INPUT_KEY_F8;
	case KEY_F9: return INPUT_KEY_F9;
	case KEY_F10: return INPUT_KEY_F10;
	case KEY_F11: return INPUT_KEY_F11;
	case KEY_F12: return INPUT_KEY_F12;
	case KEY_KP0: return INPUT_KEY_NUMPAD_0;
	case KEY_KP1: return INPUT_KEY_NUMPAD_1;
	case KEY_KP2: return INPUT_KEY_NUMPAD_2;
	case KEY_KP3: return INPUT_KEY_NUMPAD_3;
	case KEY_KP4: return INPUT_KEY_NUMPAD_4;
	case KEY_KP5: return INPUT_KEY_NUMPAD_5;
	case KEY_KP6: return INPUT_KEY_NUMPAD_6;
	case KEY_KP7: return INPUT_KEY_NUMPAD_7;
	case KEY_KP8: return INPUT_KEY_NUMPAD_8;
	case KEY_KP9: return INPUT_KEY_NUMPAD_9;
	case KEY_KPASTERISK: return INPUT_KEY_NUMPAD_MULTIPLY;
	case KEY_KPMINUS: return INPUT_KEY_NUMPAD_SUBTRACT;
	case KEY_KPPLUS: return INPUT_KEY_NUMPAD_ADD;
	case KEY_KPSLASH: return INPUT_KEY_NUMPAD_DIVIDE;
	case KEY_KPENTER: return INPUT_KEY_NUMPAD_ENTER;

	default: return INPUT_KEY_UNDEFINED;
	}
}

template <unsigned short _EventType, unsigned _MaxCodes>
static int hasBits(const int _FileDescriptorIndex, const int* const pCodes, const size_t _CodeCount) noexcept {
	uint8_t bits[(_MaxCodes + 7) / 8]{};

	if (ioctl(_FileDescriptorIndex, EVIOCGBIT(_EventType, sizeof(bits)), bits) < 0)
		return -1;

	const int* const pCodeEnd = pCodes + _CodeCount;
	for (const int* pCode{ pCodes }; pCode != pCodeEnd; ++pCode) {
		if (bits[*pCode / 8] & (1u << (*pCode % 8)))
			continue;

		return -1;
	}

	return 0;
}

struct InputDeviceImp {
	InputDeviceCapabilityFlags capability;
	udev_device* device;
	const char* devnode;
	int fileDescriptorIndex;
};

int discoverInputDevices(const InputDeviceDicoverControlInfo* const pDiscoverInfo, uint32_t* const pCount, InputDeviceSet* const pInputDeviceSet) noexcept {
	udev* _udev = nullptr;
	udev_enumerate* _enumerate = nullptr;
	mem::span<InputDeviceImp> _inputDevice;

	do {
		_udev = udev_new();

		if (!_udev)
			return -1;

		_enumerate = udev_enumerate_new(_udev);
		udev_enumerate_add_match_subsystem(_enumerate, "input");

		udev_enumerate_scan_devices(_enumerate);

		udev_list_entry* const devices = udev_enumerate_get_list_entry(_enumerate);

		InputDeviceCapabilityFlags capabilityMask = pDiscoverInfo->flags;
		
		if (!pDiscoverInfo->maxInputDevice)
			break;

		_inputDevice = mem::allocate_range<InputDeviceImp>((size_t)pDiscoverInfo->maxInputDevice);

		if (!_inputDevice)
			break;

		_inputDevice.assign_default();

		const InputDeviceImp* const pDeviceDstEnd = _inputDevice.pEnd;
		InputDeviceImp* pDeviceDst = _inputDevice.pBegin;

		udev_list_entry* entry;
		udev_list_entry_foreach(entry, devices) {
			const char* sysPath = udev_list_entry_get_name(entry);
			udev_device* device = udev_device_new_from_syspath(_udev, sysPath);

			do {
				const char* devnode = udev_device_get_devnode(device);
				if (!devnode)
					break;
				
				if (strncmp(devnode, "/dev/input/event", 16u) != 0)
					break;

				InputDeviceCapabilityFlags capability = 0u;

				if (udev_device_get_property_value(device, "ID_INPUT_KEYBOARD"))
					capability |= INPUT_DEVICE_CAPABILITY_KEYBOARD_BIT;

				if (udev_device_get_property_value(device, "ID_INPUT_MOUSE"))
					capability |= INPUT_DEVICE_CAPABILITY_MOUSE_BIT;

				if (udev_device_get_property_value(device, "ID_INPUT_GAMEPAD"))
					capability |= INPUT_DEVICE_CAPABILITY_GAMEPAD_BIT;

				capability &= capabilityMask;

				if (!capability)
					break;
				
				*pDeviceDst++ = { capability, udev_device_ref(device), devnode, -1 };

			} while (false);

			udev_device_unref(device);

			if (pDeviceDst == pDeviceDstEnd)
				break;
		}

		size_t deviceCount = static_cast<size_t>(pDeviceDst - _inputDevice.pBegin);

		InputDeviceImp* const device = mem_allocateSizeRange<InputDeviceImp>(deviceCount);

		if (!device)
			break;
		
		memcpy(device, _inputDevice.pBegin, sizeof(InputDeviceImp) * deviceCount);
		
		mem::free_range(_inputDevice);

		*pCount = deviceCount;
		*pInputDeviceSet = device;

		udev_enumerate_unref(_enumerate);
		udev_unref(_udev);

		return 0;

	} while (false);

	if (_inputDevice) {
		const InputDeviceImp* const pDeviceEnd = _inputDevice.pEnd;
		for (const InputDeviceImp* pDevice{ _inputDevice.pBegin }; pDevice != pDeviceEnd && pDevice->device; ++pDevice) {
			close(pDevice->fileDescriptorIndex);
			udev_device_unref(pDevice->device);
		}

		mem::free_range(_inputDevice);
	}

	udev_enumerate_unref(_enumerate);
	udev_unref(_udev);

	return -1;
}

void destroyInputDeviceSet(InputDeviceSet const _InputDeviceSet) noexcept {
	InputDeviceImp* pDeviceSet = reinterpret_cast<InputDeviceImp*>(_InputDeviceSet);

	const size_t deviceCount = mem_getAllocationSize<InputDeviceImp>(pDeviceSet);

	const InputDeviceImp* const pDeviceEnd = pDeviceSet + deviceCount;
	for (const InputDeviceImp* pDevice{ pDeviceSet }; pDevice != pDeviceEnd; ++pDevice)
		udev_device_unref(pDevice->device);

	mem_freeSizeRange<InputDeviceImp>(pDeviceSet);
}

void enumerateInputDeviceName(InputDeviceSet const _InputDeviceSet, const uint32_t _Count, const char** pName) noexcept {
	const InputDeviceImp* pDevice = reinterpret_cast<InputDeviceImp*>(_InputDeviceSet);

	const char** const pNameEnd = pName + _Count;
	for (;pName != pNameEnd;) {
		udev_device* inputDevice = udev_device_get_parent_with_subsystem_devtype(pDevice++->device, "input", nullptr);
		*pName++ = inputDevice ? udev_device_get_sysattr_value(inputDevice, "name") : nullptr;
	}
}

int beginInputEventPoll(InputDeviceSet const _InputDeviceSet) noexcept {
	InputDeviceImp* const pInputDevice = reinterpret_cast<InputDeviceImp*>(_InputDeviceSet);

	const size_t deviceCount = mem_getAllocationSize<InputDeviceImp>(pInputDevice);

	int fd{};

	const InputDeviceImp* const pInputDeviceEnd = pInputDevice + deviceCount;
	for (InputDeviceImp* pDevice{ pInputDevice }; pDevice != pInputDeviceEnd; ++pDevice) {
		fd = open(pDevice->devnode, O_RDONLY | O_NONBLOCK);

		if (fd < 0) {
			for (InputDeviceImp* pDevice_{ pInputDevice }; pDevice_ != pDevice; ++pDevice_) {
				close(pDevice_->fileDescriptorIndex);
				pDevice_->fileDescriptorIndex = -1;
			}

			break;
		}

		pDevice->fileDescriptorIndex = fd;
	}

	return fd < 0 ? -1 : 0;
}

void endInputEventPoll(InputDeviceSet const _InputDeviceSet) noexcept {
	InputDeviceImp* const pInputDevice = reinterpret_cast<InputDeviceImp*>(_InputDeviceSet);

	const size_t deviceCount = mem_getAllocationSize<InputDeviceImp>(pInputDevice);

	const InputDeviceImp* const pInputDeviceEnd = pInputDevice + deviceCount;
	for (InputDeviceImp* pDevice{ pInputDevice }; pDevice != pInputDeviceEnd; ++pDevice) {
		close(pDevice->fileDescriptorIndex);
		pDevice->fileDescriptorIndex = -1;
	}
}

constexpr uint32_t INPUT_EVENT_BUFFER_CAPACITY = 32u;

static int readFileEvents(int fd, input_event* const pBuffer, uint32_t* pCount) noexcept {
	constexpr size_t INPUT_EVENT_READ_SIZE = sizeof(input_event) * INPUT_EVENT_BUFFER_CAPACITY;

	ssize_t bytes = read(fd, pBuffer, INPUT_EVENT_READ_SIZE);
	
	if (bytes < 0) {
		if (errno == EINTR)
			return 1;

		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 0;

		return -1;
	}

	*pCount = static_cast<size_t>(bytes) / sizeof(input_event);

	return 0;
}

static int resolveKeyboardInputs(const InputDeviceImp* const pDevice, InputState* const pState) noexcept { while (true) {
	input_event events[INPUT_EVENT_BUFFER_CAPACITY];
	uint32_t eventCount = 0;

	int result;

	do {
		result = readFileEvents(pDevice->fileDescriptorIndex, events, &eventCount);

		if (result == -1)
			return -1;

	} while (result);

	const input_event* const pEventEnd = events + eventCount;
	for (const input_event* pEvent{ events }; pEvent != pEventEnd; ++pEvent) {
		if (pState->key.pNext == pState->key.pEnd)
			return 0;

		if (pEvent->type != EV_KEY)
			continue;

		const InputKey key = translateKeyImp(pEvent->code);

		if (key == INPUT_KEY_UNDEFINED)
			continue;
		
		InputKeyEvent* const pEventDst = pState->key.pNext++;
		pEventDst->key = key;
		pEventDst->state = translateKeyStateImp(pEvent->value);
		pEventDst->mod = pState->key.modifier;

		InputKeyModifierBit modifier = getKeyModifierBit(pEventDst->key);

		if (!modifier)
			continue;

		switch (pEvent->value) {
		case 0: pState->key.modifier &= ~modifier;
			break;

		case 1: pState->key.modifier |= modifier;
			break;
		}
	}

	if (eventCount < INPUT_EVENT_BUFFER_CAPACITY)
		return 0;
}}

static int resolveMouseInputs(const InputDeviceImp* const pDevice, InputState* const pState) noexcept { while (true) {
	input_event events[INPUT_EVENT_BUFFER_CAPACITY];
	uint32_t eventCount = 0;

	int result;

	do {
		result = readFileEvents(pDevice->fileDescriptorIndex, events, &eventCount);

		if (result == -1)
			return -1;

	} while (result);

	const input_event* const pEventEnd = events + eventCount;
	for (const input_event* pEvent{ events }; pEvent != pEventEnd; ++pEvent) {
		switch (pEvent->type) {
		case EV_KEY: {
			const InputButtons button = translateMouseButtonImp(pEvent->code);

			if (!button)
				break;

			switch (pEvent->value) {
			case 0:
				pState->cursor.button &= ~button;
				break;

			case 1:
				pState->cursor.button |= button;
				break;
			}	
		}

			break;

		case EV_REL:
			switch (pEvent->code) {
			case REL_X:
				pState->cursor.delX += pEvent->value;
				break;
					
			case REL_Y:
				pState->cursor.delY += pEvent->value;
				break;
			}

			break;
		}
	}

	if (eventCount < INPUT_EVENT_BUFFER_CAPACITY)
		return 0;
}}

void pollInputs(InputDeviceSet const _InputDeviceSet, InputState* const pInputState) noexcept {
	InputDeviceImp* const pInputDevice = reinterpret_cast<InputDeviceImp*>(_InputDeviceSet);

	const size_t deviceCount = mem_getAllocationSize<InputDeviceImp>(pInputDevice);

	const InputDeviceImp* const pInputDeviceEnd = pInputDevice + deviceCount;
	for (const InputDeviceImp* pDevice{ pInputDevice }; pDevice != pInputDeviceEnd; ++pDevice) {
		if (pDevice->capability & INPUT_DEVICE_CAPABILITY_KEYBOARD_BIT) {
			resolveKeyboardInputs(pDevice, pInputState);
		}
		else if (pDevice->capability & INPUT_DEVICE_CAPABILITY_MOUSE_BIT) {
			resolveMouseInputs(pDevice, pInputState);
		}
	}
}

#endif