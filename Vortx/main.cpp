#include "Signboard/Signboard.h"

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;

int main() {

    // WINDOW_SETUP

    plf::instance glfwInstance;

    const char* title = "NEW WINDOW";
    plf::window window{ DEFAULT_WIDTH, DEFAULT_HEIGHT, title};

    plf::windowEventState events;
    events.attachWindow(window);

    // RENDERER_SETUP

    rndr::Renderer renderer{ window };


    /*CommandContext cmdCtx = { &resource, &methods, &present, &renderer };

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
    }*/

    return EXIT_SUCCESS;
}