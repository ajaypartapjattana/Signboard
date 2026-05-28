#include "Signboard/Signboard.h"

struct CommandContext {
    rndr::resources* pResources;
    rndr::creMethods* pMethods;
    rndr::presentation* pPresentation;
    rndr::Renderer* pRenderer;
};

constexpr uint32_t MAX_COMMAND_COUNT = 32;

static bool routine_Config(CommandContext& context, glm::vec2 data) {
    context.pPresentation->validateSwapchain();
    context.pMethods->validate_primaryTarget();
    context.pRenderer->syncPresentation();

    return true;
}

static bool routine_upload(CommandContext& context, glm::vec2 data) {
    Mesher::PrimitiveInfo ngon{};
    ngon.size = 0.4f;
    ngon.res = 8;

    Mesher mesher;

    std::unique_ptr<Model> a_model = mesher.createPrimitive(MESH_PRIMITIVE::NGON, ngon);
    Model& _mdl = *a_model.get();

    uint32_t mesh = context.pResources->allocateMesh(_mdl);
    context.pRenderer->queueUpload(_mdl, mesh);

    return false;
}

static bool routine_escape(CommandContext& context, glm::vec2 data) {

    return true;
}

int main() {
    plf::context displayContext;

    plf::window::createInfo windowInfo{};
    windowInfo.extent = { 600, 800 };
    windowInfo.fullscreen = false;
    windowInfo.title = "My window";

    plf::window window{ windowInfo };
    plf::windowEventState windowState{ window };

    rndr::context context{ window };
    rndr::resources resource{ context };

    rndr::presentation present{ context, resource };

    rndr::creMethods methods{ context, present, resource };

    rndr::Renderer renderer{ context, present, methods };
    renderer.bindResources(resource);
    renderer.bindMehtods(methods);

    CommandContext cmdCtx = { &resource, &methods, &present, &renderer };

    using CommandFn = bool(*)(CommandContext&, glm::vec2);
    std::array<CommandFn, MAX_COMMAND_COUNT> commandTable;
    commandTable[(uint32_t)CommandID::CONFIG] = &routine_Config;
    commandTable[(uint32_t)CommandID::ESCAPE] = &routine_escape;

    InputMapping bindings = { {GLFW_KEY_ESCAPE, InputTrigger::Pressed, CommandID::ESCAPE} };
    std::vector<FrameCommand> toExecuteCommands;
    CommandDispatcher dispatcher{ std::move(bindings), toExecuteCommands, windowState };

    windowState.pollWindowEvents();
    while (windowState.isWindowAlive()) {
        if (!dispatcher.ensureWindowVisibility()) {
            windowState.waitWindowEvents();
            continue;
        }

        dispatcher.resolveInputEvents();

        for (FrameCommand& command : toExecuteCommands) {
            auto fn = commandTable[(uint32_t)command.id];
            if (fn && fn(cmdCtx, command.data)) {
                break;
            }
        }

        toExecuteCommands.clear();

        renderer.render();
    }

    return EXIT_SUCCESS;
}