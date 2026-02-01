#pragma once

namespace platform::procedure {
    class display_window_builder;
}

#include <GLFW/glfw3.h>

namespace platform::primitive {

    struct display_window_glfwAccess;

    class display_window {
    public:
        display_window(const display_window&) = delete;
        display_window& operator=(const display_window&) = delete;

        display_window(display_window&&) noexcept;
        display_window& operator=(display_window&&) noexcept;

        ~display_window() noexcept;

        const GLFWwindow* native_window() const noexcept;

    private:
        friend class platform::procedure::display_window_builder;
        friend struct display_window_glfwAccess;

        display_window() = default;

        GLFWwindow* m_window;
    };

}
