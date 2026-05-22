#pragma once

#include "Signboard/RHI/rhi.h"

struct VertexInputDescription {
	VkVertexInputBindingDescription binding;
	std::vector<VkVertexInputAttributeDescription> attributes;
};

namespace rhi {

	struct _pAccess {

		// --- CRE ---

		static inline VkInstance extract(const creInstance& i) noexcept { return i.m_instance; }

		static inline VkSurfaceKHR extract(const creSurface& s) noexcept { return s.m_surface; }

		static inline VkDevice extract(const creDevice& d) noexcept { return d.m_device; }
		static inline VkPhysicalDevice physicalDevice(const creDevice& d) noexcept { return d.m_physical;}
		static inline const stdQueues& queues(const creDevice& d) noexcept { return d.m_queues; }
		static inline const stdQueueFamilies& queueFamilies(const creDevice& d) noexcept {return d.m_families; }

		static inline VmaAllocator extract(const creAllocator& a) noexcept { return a.m_allocator; }
		static inline const VkPhysicalDeviceMemoryProperties& get_memProps(const creAllocator& a) noexcept { return a._memProps; }

		// --- PMV ---

		static inline VkBuffer extract(const pmvBuffer& b) noexcept { return b.m_buffer; }
		static inline VmaAllocation get_allocation(const pmvBuffer& b) noexcept { return b.allocation; }

		static inline VkCommandBuffer extract(const pmvCommandBuffer& cb) noexcept { return cb.m_commandBuffer; }
		static inline VkCommandPool get_commandPool(const pmvCommandBuffer& cb) noexcept { return cb.r_commandPool; }

		static inline VkCommandPool extract(const pmvCommandPool& cp) noexcept { return cp.m_commandPool; }
		static inline VkDevice get_device(const pmvCommandPool& cp) noexcept { return cp.r_device; }

		static inline VkDescriptorSetLayout extract(const pmvDescriptorSetLayout& dl) noexcept { return dl.m_setLayout; }

		static inline VkFramebuffer extract(const pmvFramebuffer& fb) noexcept { return fb.m_framebuffer; }
		static inline VkExtent2D extent(const pmvFramebuffer& fb) noexcept { return fb.extent; }

		static inline VkImage extract(const pmvImage& i) noexcept { return i.m_image; }
		static inline VkImageView view(const pmvImage& i) noexcept { return i.m_view; }
		static inline VmaAllocation allocation(const pmvImage& i) noexcept { return i.m_allocation; }
		static inline VkExtent3D extent(const pmvImage& i) noexcept { return i.extent; }
		static inline VkFormat format(const pmvImage& i) noexcept { return i.format; }

		static inline VkPipeline extract(const pmvPipeline& p) noexcept { return p.m_pipeline; }
		static inline VkPipelineBindPoint type(const pmvPipeline& p) noexcept { return p.type; }

		static inline VkPipelineLayout extract(const pmvPipelineLayout& pl) noexcept { return pl.m_layout; }

		static inline VkRenderPass extract(const pmvRenderPass& rp) noexcept { return rp.m_renderPass; }
		static inline uint32_t attachmentCount(const pmvRenderPass& rp) noexcept { return rp.attachmentCount; }

		static inline VkSampler extract(const pmvSampler& s) { return s.m_sampler; }

		static inline VkShaderModule extract(const pmvShader& s) noexcept { return s.m_shader; }
		static inline VkShaderStageFlagBits stage(const pmvShader& s) noexcept { return s.stage; }

		static inline VkSwapchainKHR extract(const creSwapchain& s) noexcept { return s.m_swapchain; }
		static inline VkFormat format(const creSwapchain& s) noexcept { return s.format; }
		static inline VkExtent2D extent(const creSwapchain& s) noexcept { return s.extent; }

		static VertexInputDescription generate_description(const pmvVertexLayout& vl) {
			VertexInputDescription desc{};

			desc.binding.binding = 0;
			desc.binding.stride = vl.m_stride;
			desc.binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			uint32_t attributeCount = static_cast<uint32_t>(vl.m_attributes.size());
			desc.attributes.resize(attributeCount);

			for (uint32_t i = 0; i < attributeCount; ++i) {
				const auto& attribute = vl.m_attributes[i];
				auto& att = desc.attributes[i];

				att.binding = 0;
				att.format = attribute.format;
				att.location = attribute.location;
				att.offset = attribute.offset;
			}

			return desc;
		}

	};

}