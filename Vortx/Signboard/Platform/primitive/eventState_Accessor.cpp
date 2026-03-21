#include "window_eventState.h"

namespace platform::primitive {

	eventStateInputsAccess::eventStateInputsAccess(windowEventState& eventState) noexcept 
		:
		r_eventState(eventState)
	{

	}

	size_t eventStateInputsAccess::size() const noexcept {
		return r_eventState.m_inputEvents.size();
	}

	void eventStateInputsAccess::drainInputList() noexcept {
		r_eventState.m_inputEvents.clear();
	}

	eventStateSurfaceAccess::eventStateSurfaceAccess(windowEventState& eventState) noexcept
		:
		r_eventState(eventState)
	{

	}

	const std::vector<std::string>& eventStateSurfaceAccess::fileDrops() const noexcept {
		return r_eventState.m_fileDrops;
	}

	uint64_t eventStateSurfaceAccess::windowResizeState() const noexcept {
		return r_eventState.windowResize;
	}

	void eventStateSurfaceAccess::clearFileDrops() noexcept {
		r_eventState.m_fileDrops.clear();
	}

	void eventStateSurfaceAccess::consumeResize() noexcept {
		r_eventState.windowResize = frame::event::encodeResize(false, 0, 0);
	}

}