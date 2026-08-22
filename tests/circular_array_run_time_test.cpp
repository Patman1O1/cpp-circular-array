// ISO C++ Includes
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <expected>
#include <functional>
#include <limits>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Google Test Includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Local Includes
#include <collections/circular_array.hpp>

namespace collections::circular_array_testing {
    namespace {
        // ── Concepts ────────────────────────────────────────────────────────
        template<typename T, typename... Args>
        concept brace_initializable = requires { T{std::declval<Args>()...}; };

        template<typename... Args>
        concept deducible = requires {
            circular_array{std::declval<Args>()...};
        };

    } // namespace

    namespace iterator_testing {
        namespace constructor_testing {

        } // namespace constructor_testing

        namespace overloaded_operator_testing {
            TEST(
                circular_array_iterator_overloaded_operators,
                prefix_increment_operator_single_element_array
            ) {
                circular_array arr = {1};
                auto it = arr.begin();

                EXPECT_EQ(1, *it);

                ++it;
                EXPECT_EQ(1, *it);

                ++it;
                EXPECT_EQ(1, *it);
            }
        } // namespace overloaded_operator_testing
    } // namespace iterator_testing
} // namespace collections::circular_array_testing
