#include "Signboard/Signboard.h"

#include "Signboard/Assets/io/io.h"

int main() {
    io::ImageFile image;
    io::Result result = image.loadPNGw(L"assets_data/textures/seaside.png");

    return EXIT_SUCCESS;

    Platform platform;

    platform.createWindowClass();

    HWND window;
    {
        const wchar_t* title = L"PLATFORM_WINDOW";

        constexpr uint32_t width = 800;
        constexpr uint32_t height = 600;

        window = platform.createWindow(title, width, height);
    }

    Renderer renderer;

    try {
        renderer.deploy();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    HINSTANCE instance = platform.getInstance();

    {
        size_t deviceCount;
        renderer.enumeratePhysicalDevices(&deviceCount, nullptr);

        std::vector<const char*> devices(deviceCount);
        renderer.enumeratePhysicalDevices(&deviceCount, devices.data());

        for (size_t i = 0; i < deviceCount; ++i) {
            std::cout << "[" << i << "] : " << devices[i] << std::endl;
        }

        std::cout << "PHYSICAL_DEVICE_INDEX : ";
        
        uint32_t selectedDeviceIndex = 0;
        std::cin >> selectedDeviceIndex;


        try {
            renderer.createDevice(instance, window, selectedDeviceIndex);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    renderer.pushRenderTarget(instance, window);

    while (platform.pollEvents()) {

    }

    renderer.reset();

    return EXIT_SUCCESS;
}