// ISO C Includes
#include <cstddef>

// ISO C++ Includes
#include <exception>
#include <iostream>

// Local Includes
#include <collections/circular_array.hpp>

auto main() -> int {
    try {

        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
