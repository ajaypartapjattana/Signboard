#include "pushConstantRange.h"

namespace rhi {

	pmvPushConstantRange::pmvPushConstantRange() noexcept
		: 
		m_range()
	{

	}

	pmvPushConstantRange::pmvPushConstantRange(pmvPushConstantRange&& other) noexcept
		:
		m_range(other.m_range)
	{

	}

	pmvPushConstantRange& pmvPushConstantRange::operator=(pmvPushConstantRange&& other) noexcept {
		if (this == &other)
			return *this;

		m_range = other.m_range;

		return *this;
	}

	pmvPushConstantRange::~pmvPushConstantRange() noexcept {

	}

}