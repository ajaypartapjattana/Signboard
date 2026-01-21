#pragma once

#include "Signboard/Core/Interfaces/startupConfigType.h"

#include <string>
#include <iostream>
#include <filesystem>

namespace window::procedure {
    class display_window_builder;
}

namespace window::primitive {

    struct display_window_glfwAccess;

    class display_window {
    public:
        display_window(const display_window&) = delete;
        display_window& operator=(const display_window&) = delete;

        display_window(display_window&&) noexcept;
        display_window& operator=(display_window&&) noexcept;

        ~display_window() noexcept;

        const void* native_window() const noexcept;

    private:
        friend class window::procedure::display_window_builder;
        friend struct display_window_glfwAccess;

        display_window() = default;

        void* m_window;
    };

}
