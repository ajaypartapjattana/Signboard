#pragma once

#include "Signboard/resources/common/MeshSystemTypes.h"
#include "Signboard/RHI/vulkan/VulkanBuffer.h"

#include <vector>
#include <memory>

struct Mesh;

class VulkanDevice;
class VulkanCommandBuffer;

class MeshSystem {
public:
	explicit MeshSystem(VulkanDevice& device);
	~MeshSystem();	

	MeshHandle createMesh(VulkanCommandBuffer& cmd, const MeshDesc& desc);
	const Mesh& get(MeshHandle handle) const;

	void destroy(MeshHandle handle);
	void flushDeletes();

private:
	VulkanBuffer createVertexBuffer();
	VulkanBuffer createIndexBuffer();

	MeshHandle allocateSlot(std::unique_ptr<Mesh> mesh);

private:
	VulkanDevice& device;

	VulkanBuffer vertexBuffer;
	VulkanBuffer indexBuffer;

	uint64_t vertexCursor = 0;
	uint64_t indexCursor = 0;

	struct Slot	{
		std::unique_ptr<Mesh> mesh;
		uint32_t generation = 0;
	};

	std::vector<Slot> slots;
	std::vector<uint32_t> freeList;

	std::vector<uint32_t> pendingDeletes;

};