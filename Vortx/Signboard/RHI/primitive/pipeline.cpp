#include "pipeline.h"

namespace rhi {

	pmvPipeline::pmvPipeline() noexcept 
		: 
		m_pipeline(VK_NULL_HANDLE), 
		m_type(VK_PIPELINE_BIND_POINT_GRAPHICS),
		m_device(VK_NULL_HANDLE)
	{

	}

	pmvPipeline::pmvPipeline(pmvPipeline&& other) noexcept
		:
		m_pipeline(other.m_pipeline),
		m_type(other.m_type),
		m_device(other.m_device)
	{
		other.m_pipeline = VK_NULL_HANDLE;
	}

	pmvPipeline& pmvPipeline::operator=(pmvPipeline&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_pipeline)
			vkDestroyPipeline(m_device, m_pipeline, nullptr);

		m_pipeline = other.m_pipeline;
		m_type = other.m_type;
		m_device = other.m_device;

		other.m_pipeline = VK_NULL_HANDLE;

		return *this;
	}

	pmvPipeline::~pmvPipeline() noexcept {
		if (m_pipeline)
			vkDestroyPipeline(m_device, m_pipeline, nullptr);
	}

}