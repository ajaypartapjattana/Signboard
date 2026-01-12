#include "Signboard/Signboard.h"

#include <iostream>

int main() {
    Signboard board;
    try {
        board.run();
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << e.what() << std::endl << std::endl << "EXECUTION ABORTED" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}