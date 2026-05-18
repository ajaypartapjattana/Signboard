#include "pipeline.h"

namespace rhi {

	pmvPipeline::pmvPipeline() noexcept 
		: 
		m_pipeline(VK_NULL_HANDLE), 
		type(VK_PIPELINE_BIND_POINT_GRAPHICS),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvPipeline::pmvPipeline(pmvPipeline&& other) noexcept
		:
		m_pipeline(other.m_pipeline),
		type(other.type),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	pmvPipeline& pmvPipeline::operator=(pmvPipeline&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyPipeline(r_device, m_pipeline, nullptr);

		m_pipeline = other.m_pipeline;
		type = other.type;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvPipeline::~pmvPipeline() noexcept {
		if (r_device)
			vkDestroyPipeline(r_device, m_pipeline, nullptr);
	}

	void pmvPipeline::reset() noexcept {
		if (r_device) {
			vkDestroyPipeline(r_device, m_pipeline, nullptr);
			r_device = VK_NULL_HANDLE;
		}
		
		m_pipeline = VK_NULL_HANDLE;
	}

}