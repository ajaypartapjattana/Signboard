#include "Signboard/Signboard.h"

#include <iostream>

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