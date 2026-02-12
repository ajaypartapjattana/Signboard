#include "pipeline.h"

namespace rhi::primitive {

	pipeline::pipeline() noexcept 
		: m_pipeline(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{

	}

	pipeline::pipeline(pipeline&& other) noexcept {
		m_pipeline = other.m_pipeline;
		m_device = other.m_device;

		other.m_pipeline = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	pipeline& pipeline::operator=(pipeline&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_pipeline)
			vkDestroyPipeline(m_device, m_pipeline, nullptr);

		m_pipeline = other.m_pipeline;
		m_device = other.m_device;

		other.m_pipeline = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	pipeline::~pipeline() noexcept {
		if (m_pipeline)
			vkDestroyPipeline(m_device, m_pipeline, nullptr);
	}

	VkPipeline pipeline::native_pipeline() const noexcept {
		return m_pipeline;
	}

}