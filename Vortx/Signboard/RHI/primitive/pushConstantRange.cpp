#include "pushConstantRange.h"

namespace rhi::primitive {

	pushConstantRange::pushConstantRange() noexcept
		: m_range()
	{

	}

	pushConstantRange::pushConstantRange(pushConstantRange&& other) noexcept{
		m_range = other.m_range;
	}

	pushConstantRange& pushConstantRange::operator=(pushConstantRange&& other) noexcept {
		if (this == &other)
			return *this;

		m_range = other.m_range;

		return *this;
	}

	pushConstantRange::~pushConstantRange() noexcept {

	}

	VkPushConstantRange pushConstantRange::native_pushConstantRange() const noexcept {
		return m_range;
	}

}