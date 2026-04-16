#include "Signboard.h"

Signboard::Signboard()
	:
	m_context(m_target.native_target()),
	m_resources(m_context),
	m_scene(),

	m_renderer(m_context, m_resources, m_scene),

	targetVisible(true),

	m_dispatcher(bindings, toExecuteCommands, targetVisible)
{
	populateCommandTable();
	setupDefaults();
}

void Signboard::setupDefaults() {
	Mesher l_mesher;
	Mesher::PrimitiveInfo _info{};
	_info.size = 0.5f;
	_info.res = 16;

	auto _defModel = l_mesher.createPrimitive(MESH_PRIMITIVE::NGON, _info);
	Model& r_defModel = *_defModel.get();

	uint32_t allocatedMesh = m_resources.allocateMesh(r_defModel);
	m_renderer.queueUpload(r_defModel, allocatedMesh);
}

void Signboard::populateCommandTable() {
	commandTable[(uint32_t)CommandID::CONFIG] = &Signboard::routine_Config;
}

void Signboard::run() {
	while (m_target.isAvailable()) {
		m_dispatcher.resolveSurfaceEvents(m_target.get_eventState());

		if (!targetVisible) {
			m_target.waitForEvents();
			continue;
		}

		m_dispatcher.resolveInputEvents(m_target.get_eventState());

		if (!executeCommands())
			continue;

		m_renderer.render();

	}
}

bool Signboard::executeCommands() {
	for (FrameCommand& CMDGraphics : toExecuteCommands) {
		auto fn = commandTable[(uint32_t)CMDGraphics.id];
		if (fn && fn(*this, CMDGraphics.data)) {
			toExecuteCommands.clear();
			return false;
		}
	}

	toExecuteCommands.clear();
	return true;
}

bool Signboard::routine_Config(Signboard& board, glm::vec2 data) {
	board.m_renderer.configurePresentation(nullptr);
	return true;
}

bool Signboard::routine_upload(Signboard& board, glm::vec2 data) {
	Mesher::PrimitiveInfo _info{};
	_info.size = 0.4f;
	_info.res = 8;

	std::unique_ptr<Model> a_model = board.m_mesher.createPrimitive(MESH_PRIMITIVE::CUBE, _info);
	Model& _mdl = *a_model.get();

	uint32_t mesh = board.m_resources.allocateMesh(_mdl);
	board.m_renderer.queueUpload(_mdl, mesh);

	return false;
}

	