#include "MeshSystem.h"
#include "primitive/Mesh.h"

#include <stdexcept>
#include <cassert>

MeshSystem::MeshSystem(VulkanDevice& device)
	: device(device),
	vertexBuffer(createVertexBuffer()), 
	indexBuffer(createIndexBuffer())
{
	
}

VulkanBuffer MeshSystem::createVertexBuffer() {
	BufferDesc vertB;
	vertB.size = 256 * 1024 * 1024;
	vertB.usageFlags.set(BufferUsage::Vertex, BufferUsage::TransferDestination);
	vertB.memoryFlags = MemoryProperty::DeviceLocal;

	return VulkanBuffer(device, vertB);
}

VulkanBuffer MeshSystem::createIndexBuffer() {
	BufferDesc vertB;
	vertB.size = 64 * 1024 * 1024;
	vertB.usageFlags.set(BufferUsage::Index, BufferUsage::TransferDestination);
	vertB.memoryFlags = MemoryProperty::DeviceLocal;

	return VulkanBuffer(device, vertB);
}

MeshSystem::~MeshSystem() {
	slots.clear();
	freeList.clear();
}

MeshHandle MeshSystem::allocateSlot(std::unique_ptr<Mesh> mesh) {
	uint32_t index;

	if (!freeList.empty()) {
		index = freeList.back();
		freeList.pop_back();
	} else {
		index = static_cast<uint32_t>(slots.size());
		slots.emplace_back();
	}

	Slot& slot = slots[index];
	slot.mesh = std::move(mesh);

	return MeshHandle{ index, slot.generation };
}

const Mesh& MeshSystem::get(MeshHandle handle) const {
	const Slot& slot = slots[handle.index];

	assert(slot.mesh && "MeshHandle refers to destroyed mesh");
	assert(slot.generation == handle.generation && "MeshHandle generation mismatch");

	return *slot.mesh;
}

void MeshSystem::destroy(MeshHandle handle) {
	uint32_t index = handle.index;

	Slot& slot = slots[index];
	if (slot.generation != handle.generation)
		return;

	pendingDeletes.push_back(index);
}

void MeshSystem::flushDeletes() {
	for (uint32_t index : pendingDeletes) {
		Slot& slot = slots[index];

		if (!slot.mesh) continue;
		slot.mesh.reset();
		++slot.generation;
		freeList.push_back(index);
	}
	pendingDeletes.clear();
}

MeshHandle MeshSystem::createMesh(VulkanCommandBuffer& cmd, const MeshDesc& desc) {
	if (!desc.p_vertexData || !desc.p_indexData || desc.vertexCount == 0 || desc.indexCount == 0)
		return INVALID_MESH; 

	uint64_t vertexBufferSize = desc.vertexCount * desc.vertexSize;
	uint64_t indexBufferSize = desc.indexCount * sizeof(uint32_t);

	uint64_t vertexOffset = alignUp(vertexCursor, 16);
	uint64_t indexOffset = alignUp(indexCursor, 4);

	if (vertexCursor + vertexBufferSize > vertexBuffer.getSize() || indexCursor + indexBufferSize > indexBuffer.getSize())
		return INVALID_MESH;

	vertexCursor += vertexBufferSize;
	indexCursor += indexBufferSize;

	BufferDesc stagingVertexDesc{};
	stagingVertexDesc.size = vertexBufferSize;
	stagingVertexDesc.memoryFlags.set(MemoryProperty::HostVisible, MemoryProperty::HostCoherent);
	stagingVertexDesc.usageFlags = BufferUsage::TransferSource;

	VulkanBuffer stagingVertex(device, stagingVertexDesc);
	stagingVertex.upload(desc.p_vertexData, vertexBufferSize);

	BufferDesc stagingIndexDesc{};
	stagingIndexDesc.size = indexBufferSize;
	stagingIndexDesc.memoryFlags.set(MemoryProperty::HostVisible, MemoryProperty::HostCoherent);
	stagingIndexDesc.usageFlags = BufferUsage::TransferSource;

	VulkanBuffer stagingIndex(device, stagingIndexDesc);
	stagingIndex.upload(desc.p_indexData, indexBufferSize);

	vertexBuffer.copyFrom(cmd, stagingVertex, vertexBufferSize, 0, vertexOffset);
	indexBuffer.copyFrom(cmd, stagingIndex, indexBufferSize, 0, indexOffset);

	auto mesh = std::make_unique<Mesh>(vertexOffset, indexOffset, desc.vertexCount, desc.indexCount);

	return allocateSlot(std::move(mesh));
}

constexpr uint64_t alignUp(uint64_t value, uint64_t alignment) {
	return (value + alignment - 1) & ~(alignment - 1);
}