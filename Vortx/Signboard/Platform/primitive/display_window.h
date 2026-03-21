#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace platform::primitive {

    struct displayWindow_pAccess;

    class displayWindow {
    public:
        struct createInfo {
            std::string title;

            bool fullscreen = false;
            struct Extent2D {
                uint32_t width;
                uint32_t height;
            } extent;
        };

        displayWindow(const createInfo& createInfo);

        displayWindow(const displayWindow&) = delete;
        displayWindow& operator=(const displayWindow&) = delete;

        displayWindow(displayWindow&&) noexcept;
        displayWindow& operator=(displayWindow&&) noexcept;

        ~displayWindow() noexcept;

        const GLFWwindow* native_window() const noexcept;

    private:
        friend struct displayWindow_pAccess;

        GLFWwindow* m_window;
    };

}
