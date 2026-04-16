#include "Signboard/Signboard.h"

#include <iostream>
#include <external/spirv_cross/spirv_cross.hpp>

int main() {
    try {
        Signboard board;
        board.run();
    }
    catch (const std::exception& e) {
        std::cerr << '\n' << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}