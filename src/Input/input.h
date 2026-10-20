#pragma once

#include <cstdint>

#include <Platform/input.h>

constexpr size_t KEY_PAGE_COUNT = (KEY_IMP + 63u) >> 6;

struct InputKeyField {
	uint64_t repeat[KEY_PAGE_COUNT];
	uint64_t press[KEY_PAGE_COUNT];
	uint64_t release[KEY_PAGE_COUNT];
};

constexpr inline bool isKeyPressed(const InputKeyField* const pKeyField, const Key key) noexcept {
	const size_t keyPage = key >> 6;
	const uint64_t keyBit = (uint64_t)1u << (key & 0x3F);

	return pKeyField->press[keyPage] & keyBit;
}

constexpr inline bool isKeyReleased(const InputKeyField* const pKeyField, const Key key) noexcept {
	const size_t keyPage = key >> 6;
	const uint64_t keyBit = (uint64_t)1u << (key & 0x3F);

	return pKeyField->release[keyPage] & keyBit;
}

constexpr inline bool isKeyDown(const InputKeyField* const pKeyField, const Key key) noexcept {
	const size_t keyPage = key >> 6;
	const uint64_t keyBit = (uint64_t)1u << (key & 0x3F);

	return pKeyField->repeat[keyPage] & keyBit;
}


struct InputDragField {
	uint32_t repeat;
	uint32_t press;
	uint32_t release;
	int32_t delta[2];
	int32_t tchSt[2];
	int32_t tchLt[2];
};

enum InputDeviceCapability : uint32_t {
	INPUT_DEVICE_CAPABILITY_UNDEFINED,
    INPUT_DEVICE_CAPABILITY_KEYBOARD,
    INPUT_DEVICE_CAPABILITY_MOUSE,
	INPUT_DEVICE_CAPABILITY_TOUCHPAD,
    INPUT_DEVICE_CAPABILITY_GAMEPAD
};

using InputDeviceSet = void*;

struct InputDeviceDicoverControlInfo {
	uint32_t maxInputDevice;
	uintptr_t window;
};

int discoverInputDevices(const InputDeviceDicoverControlInfo* pDiscoverInfo, uint32_t* pCount, InputDeviceSet* pInputDeviceSet) noexcept;
void destroyInputDeviceSet(InputDeviceSet _InputDeviceSet) noexcept;

void enumerateInputDeviceName(InputDeviceSet _InputDeviceSet, uint32_t _Count, const char** pName) noexcept;

int beginInputEventPoll(InputDeviceSet _InputDeviceSet) noexcept;
void endInputEventPoll(InputDeviceSet _InputDeviceSet) noexcept;

void pollInputs(InputDeviceSet _InputDeviceSet, InputDragField* pDragField, InputKeyField* pKeyField) noexcept;
