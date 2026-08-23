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

    // ── Aggregate Tests ─────────────────────────────────────────────────────
    namespace aggregate_testing {
        TEST(circular_array_aggregate, is_aggregate) {
            static_assert(std::is_aggregate_v<circular_array<int, 3>>);
            static_assert(std::is_aggregate_v<circular_array<std::string, 2>>);

            SUCCEED();
        }

        TEST(circular_array_aggregate, is_brace_initializable) {
            static_assert(
                brace_initializable<circular_array<int, 3>, int, int, int>
            );
            static_assert(
                brace_initializable<circular_array<int, 3>, int, int>
            );
            static_assert(brace_initializable<circular_array<int, 3>, int>);
            static_assert(brace_initializable<circular_array<int, 3>>);
            static_assert(
                !brace_initializable<circular_array<int, 3>, int, int, int, int>
            );
            static_assert(
                !brace_initializable<circular_array<int, 2>, int, std::string>
            );

            SUCCEED();
        }

        TEST(circular_array_aggregate, no_narrowing_conversions) {
            static_assert(
                !brace_initializable<circular_array<int, 2>, double, double>
            );
            static_assert(
                !brace_initializable<circular_array<int, 2>, int, long long>
            );
            static_assert(
                !brace_initializable<circular_array<char, 2>, int, int>
            );
            static_assert(
                !brace_initializable<circular_array<double, 2>, int, int>
            );

            SUCCEED();
        }

        TEST(circular_array_aggregate, initializes_remaining_elements) {
            constexpr circular_array<int, 3> values = {7};
            static_assert(7 == values[0]);
            static_assert(0 == values[1]);
            static_assert(0 == values[2]);

            SUCCEED();
        }

        TEST(circular_array_aggregate, no_implicit_conversions) {
            static_assert(!std::is_convertible_v<int, circular_array<int, 3>>);

            SUCCEED();
        }

        TEST(circular_array_aggregate, is_default_constructible) {
            static_assert(
                std::is_default_constructible_v<circular_array<int, 3>>
            );
            static_assert(
                std::is_default_constructible_v<circular_array<std::string, 3>>
            );
            static_assert(
                std::is_trivially_default_constructible_v<circular_array<int, 3>>
            );
            static_assert(
                !std::is_trivially_default_constructible_v<
                    circular_array<std::string, 3>>
            );

            SUCCEED();
        }

        TEST(circular_array_aggregate, is_copy_constructible) {
            static_assert(
                std::is_copy_constructible_v<circular_array<int, 3>>
            );
            static_assert(
                std::is_copy_constructible_v<circular_array<std::string, 2>>
            );
            static_assert(
                std::is_trivially_copy_constructible_v<circular_array<int, 3>>
            );
            static_assert(
                !std::is_trivially_copy_constructible_v<
                    circular_array<std::string, 2>>
            );

            SUCCEED();
        }

        TEST(circular_array_aggregate, copy_construction) {
            circular_array<int, 3> src = {1, 2, 3};
            circular_array<int, 3> dst = src;

            dst[0] = 99;

            EXPECT_EQ(1, src[0]);
            EXPECT_EQ(99, dst[0]);
        }

        TEST(circular_array_aggregate, copy_construction_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            circular_array<std::string, 2> src = {std::string(cstr), "beta"};
            circular_array<std::string, 2> dst = src;

            EXPECT_EQ(src[0], dst[0]);
            EXPECT_EQ(src[1], dst[1]);
            EXPECT_NE(src.data(), dst.data());
        }

        TEST(circular_array_aggregate, is_move_constructible) {
            static_assert(
                std::is_move_constructible_v<circular_array<int, 3>>
            );
            static_assert(
                std::is_move_constructible_v<circular_array<std::string, 2>>
            );
            static_assert(
                std::is_trivially_move_constructible_v<circular_array<int, 3>>
            );
            static_assert(
                !std::is_trivially_move_constructible_v<
                    circular_array<std::string, 2>>
            );

            SUCCEED();
        }

        TEST(circular_array_aggregate, move_construction_transfers_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            circular_array<std::string, 1> src = {std::string(cstr)};
            circular_array<std::string, 1> dst = std::move(src);

            EXPECT_EQ(dst[0], cstr);
            EXPECT_NE(src[0], cstr);  // moved-from
        }

        TEST(circular_array_aggregate, is_destructible) {
            static_assert(std::is_destructible_v<circular_array<int, 3>>);
            static_assert(
                std::is_destructible_v<circular_array<std::string, 3>>
            );
            static_assert(
                std::is_trivially_destructible_v<circular_array<int, 3>>
            );
            static_assert(
                !std::is_trivially_destructible_v<
                    circular_array<std::string, 3>>
            );

            SUCCEED();
        }

        TEST(circular_array_aggregate, is_standard_layout) {
            static_assert(std::is_standard_layout_v<circular_array<int, 3>>);
            static_assert(sizeof(circular_array<int, 3>) == sizeof(int[3]));
            static_assert(alignof(circular_array<int, 3>) == alignof(int[3]));

            SUCCEED();
        }

        TEST(circular_array_aggregate, is_deducible) {
            circular_array values = {1, 2, 3};
            static_assert(
                std::same_as<decltype(values), circular_array<int, 3>>
            );

            [[maybe_unused]] circular_array singleton = {1};
            static_assert(
                std::same_as<decltype(singleton), circular_array<int, 1>>
            );

            static_assert(deducible<int, int, int>);
            static_assert(!deducible<int, double>);

            EXPECT_EQ(1, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(3, values[2]);
        }

        TEST(circular_array_aggregate, is_structural) {
            static_assert(std::is_structural_v<circular_array<int, 3>>);

            SUCCEED();
        }
    } // namespace aggregate_testing

    // ── Overloaded Operators Tests ──────────────────────────────────────────
    namespace overloaded_operators_testing {
        TEST(circular_array_operators, is_copy_assignable) {
            static_assert(
                std::is_copy_assignable_v<circular_array<int, 3>>
            );
            static_assert(
                std::is_copy_assignable_v<circular_array<std::string, 3>>
            );
            static_assert(
                std::is_trivially_copy_assignable_v<circular_array<int, 3>>
            );
            static_assert(
                !std::is_trivially_copy_assignable_v<
                    circular_array<std::string, 3>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, copy_assignment_is_noexcept_for_trivial) {
            static_assert(
                std::is_nothrow_copy_assignable_v<circular_array<int, 3>>
            );

            static_assert(
                !std::is_nothrow_copy_assignable_v<
                    circular_array<std::string, 2>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, copy_assignment_compile_time) {
            static_assert(
                []() -> bool {
                    circular_array<int, 3> src = {1, 2, 3};
                    circular_array<int, 3> dst = {4, 5, 6};
                    dst = src;
                    return dst[0] == 1 && dst[1] == 2 && dst[2] == 3 &&
                           src[0] == 1 && src[1] == 2 && src[2] == 3;
                }()
            );
        }

        TEST(circular_array_operators, copy_assignment_runtime) {
            circular_array<int, 3> src = {1, 2, 3};
            circular_array<int, 3> dst = {4, 5, 6};

            dst = src;

            EXPECT_EQ(1, dst[0]);
            EXPECT_EQ(2, dst[1]);
            EXPECT_EQ(3, dst[2]);
            EXPECT_EQ(1, src[0]);
            EXPECT_EQ(2, src[1]);
            EXPECT_EQ(3, src[2]);
        }

        TEST(circular_array_operators, copy_assignment_does_not_share_storage) {
            circular_array<int, 3> src = {1, 2, 3};
            circular_array<int, 3> dst = {4, 5, 6};

            dst = src;
            dst[0] = 99;

            EXPECT_EQ(1, src[0]);
        }

        TEST(circular_array_operators, copy_assignment_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            circular_array<std::string, 2> src = {std::string(cstr), "beta"};
            circular_array<std::string, 2> dst;

            dst = src;

            EXPECT_EQ(cstr,   dst[0]);
            EXPECT_EQ("beta", dst[1]);
            EXPECT_NE(src.data(), dst.data());
        }

        TEST(circular_array_operators, copy_assignment_returns_self_reference) {
            circular_array<int, 2> a = {1, 2};
            circular_array<int, 2> b = {3, 4};
            circular_array<int, 2> c = {5, 6};

            // Chained assignment
            c = b = a;
            EXPECT_EQ(1, c[0]);
            EXPECT_EQ(2, c[1]);
        }

        TEST(circular_array_operators, is_move_assignable) {
            static_assert(
                std::is_move_assignable_v<circular_array<int, 3>>
            );
            static_assert(
                std::is_move_assignable_v<circular_array<std::string, 3>>
            );
            static_assert(
                std::is_trivially_move_assignable_v<circular_array<int, 3>>
            );
            static_assert(
                !std::is_trivially_move_assignable_v<
                    circular_array<std::string, 3>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, move_assignment_is_noexcept) {
            static_assert(
                std::is_nothrow_move_assignable_v<circular_array<int, 3>>
            );
            static_assert(
                std::is_nothrow_move_assignable_v<
                    circular_array<std::string, 3>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, move_assignment_runtime_trivial) {
            circular_array<int, 3> src = {1, 2, 3};
            circular_array<int, 3> dst = {4, 5, 6};

            dst = std::move(src);

            EXPECT_EQ(1, dst[0]);
            EXPECT_EQ(2, dst[1]);
            EXPECT_EQ(3, dst[2]);
        }

        TEST(circular_array_operators, move_assignment_runtime_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            circular_array<std::string, 1> src = {std::string(cstr)};
            circular_array<std::string, 1> dst;

            dst = std::move(src);

            EXPECT_EQ(cstr, dst[0]);
            EXPECT_NE(cstr, src[0]);
        }

        TEST(circular_array_operators, move_assignment_returns_self_reference) {
            circular_array<int, 2> src = {1, 2};
            circular_array<int, 2> dst = {3, 4};

            circular_array<int, 2>& ref = (dst = std::move(src));
            EXPECT_EQ(&dst, &ref);
        }

        TEST(circular_array_operators, equality_trivial_equal) {
            constexpr circular_array<int, 3> a = {1, 2, 3};
            constexpr circular_array<int, 3> b = {1, 2, 3};

            static_assert(a == b);
            static_assert(!(a != b));

            EXPECT_TRUE(a == b);
            EXPECT_FALSE(a != b);
        }

        TEST(circular_array_operators, equality_trivial_not_equal) {
            constexpr circular_array<int, 3> a = {1, 2, 3};
            constexpr circular_array<int, 3> b = {1, 2, 4};

            static_assert(!(a == b));
            static_assert(a != b);

            EXPECT_FALSE(a == b);
            EXPECT_TRUE(a != b);
        }

        TEST(circular_array_operators, equality_non_trivial) {
            circular_array<std::string, 2> a = {"hello", "world"};
            circular_array<std::string, 2> b = {"hello", "world"};
            circular_array<std::string, 2> c = {"hello", "there"};

            EXPECT_TRUE(a == b);
            EXPECT_FALSE(a == c);
        }

        TEST(circular_array_operators, equality_self) {
            constexpr circular_array<int, 3> a = {1, 2, 3};
            static_assert(a == a);

            circular_array<std::string, 2> b = {"x", "y"};
            EXPECT_TRUE(b == b);
        }

        TEST(circular_array_operators, equality_single_element) {
            constexpr circular_array<int, 1> a = {42};
            constexpr circular_array<int, 1> b = {42};
            constexpr circular_array<int, 1> c = {0};

            static_assert(a == b);
            static_assert(a != c);
        }

        TEST(circular_array_operators, spaceship_equal) {
            constexpr circular_array<int, 3> a = {1, 2, 3};
            constexpr circular_array<int, 3> b = {1, 2, 3};

            static_assert((a <=> b) == 0);
            static_assert(!(a < b));
            static_assert(!(a > b));
            static_assert(a <= b);
            static_assert(a >= b);

            SUCCEED();
        }

        TEST(circular_array_operators, spaceship_less_than_first_element) {
            constexpr circular_array<int, 3> a = {1, 2, 3};
            constexpr circular_array<int, 3> b = {2, 2, 3};

            static_assert((a <=> b) < 0);
            static_assert(a < b);
            static_assert(a <= b);
            static_assert(!(a > b));

            SUCCEED();
        }

        TEST(circular_array_operators, spaceship_less_than_last_element) {
            constexpr circular_array<int, 3> a = {1, 2, 3};
            constexpr circular_array<int, 3> b = {1, 2, 4};

            static_assert((a <=> b) < 0);
            static_assert(a < b);

            SUCCEED();
        }

        TEST(circular_array_operators, spaceship_greater_than) {
            constexpr circular_array<int, 3> a = {1, 3, 3};
            constexpr circular_array<int, 3> b = {1, 2, 3};

            static_assert((a <=> b) > 0);
            static_assert(a > b);
            static_assert(a >= b);

            SUCCEED();
        }

        TEST(circular_array_operators, spaceship_single_element) {
            constexpr circular_array<int, 1> a = {1};
            constexpr circular_array<int, 1> b = {2};

            static_assert(a < b);
            static_assert(b > a);

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_mut_returns_mut_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>()[0]),
                    circular_array<int, 3>::reference>
            );
            static_assert(
                !std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<circular_array<int, 3>&>()[0])>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_mut_is_noexcept) {
            static_assert(noexcept(std::declval<circular_array<int, 3>&>()[0]));

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_mut_compile_time) {
            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values[0] = 10;
                return 10 == values[0] && 2 == values[1] && 3 == values[2];
            }());

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_mut_aliases_storage) {
            circular_array values = {1, 2, 3};

            const int& first  = values[0];
            int&       second = values[1];

            EXPECT_EQ(1, &second - &first);

            second = 20;
            EXPECT_EQ(20, values[1]);
        }

        TEST(circular_array_operators, subscript_const_returns_const_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>()[0]),
                    circular_array<int, 3>::const_reference>
            );
            static_assert(
                std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<const circular_array<int, 3>&>()[0])>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_const_is_noexcept) {
            static_assert(
                noexcept(std::declval<const circular_array<int, 3>&>()[0])
            );

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_const_compile_time) {
            constexpr circular_array values = {1, 2, 3};
            static_assert(1 == values[0]);
            static_assert(2 == values[1]);
            static_assert(3 == values[2]);

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_const_aliases_storage) {
            constexpr circular_array values = {1, 2, 3};

            const int& first  = values[0];
            const int& second = values[1];

            EXPECT_EQ(1, &second - &first);
        }

        TEST(circular_array_operators, subscript_const_does_not_copy_non_trivial) {
            const circular_array values = {
                std::string("alpha"), std::string("beta")
            };

            EXPECT_EQ("alpha", values[0]);
            EXPECT_EQ(values.data(), &values[0]);
        }

        TEST(circular_array_operators, subscript_wraps_forward) {
            constexpr circular_array values = {10, 20, 30};

            // Index N aliases index 0
            static_assert(10 == values[3]);
            static_assert(20 == values[4]);
            static_assert(30 == values[5]);
            static_assert(10 == values[6]);

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_wraps_backward) {
            constexpr circular_array values = {10, 20, 30};

            // Index -1 aliases index N-1
            static_assert(30 == values[-1]);
            static_assert(20 == values[-2]);
            static_assert(10 == values[-3]);
            static_assert(30 == values[-4]);

            SUCCEED();
        }

        TEST(circular_array_operators, subscript_wrap_mut_allows_write) {
            circular_array values = {10, 20, 30};

            values[3] = 100;
            EXPECT_EQ(100, values[0]);

            values[-1] = 300;
            EXPECT_EQ(300, values[2]);
        }

        TEST(circular_array_operators, subscript_wrap_single_element) {
            constexpr circular_array<int, 1> values = {42};

            static_assert(42 == values[0]);
            static_assert(42 == values[1]);
            static_assert(42 == values[100]);
            static_assert(42 == values[-1]);
            static_assert(42 == values[-99]);

            SUCCEED();
        }

    } // namespace overloaded_operators_testing

    // ── Method Tests ────────────────────────────────────────────────────────
    namespace methods_testing {
        TEST(circular_array_methods, at_mut_returns_mut_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().at(0)),
                    circular_array<int, 3>::reference>
            );
            static_assert(
                !std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<circular_array<int, 3>&>().at(0))>>
            );

            SUCCEED();
        }

        TEST(circular_array_methods, at_mut_is_noexcept) {
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().at(0)));

            SUCCEED();
        }

        TEST(circular_array_methods, at_const_returns_const_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().at(0)),
                    circular_array<int, 3>::const_reference>
            );
            static_assert(
                std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<const circular_array<int, 3>&>().at(0))>>
            );

            SUCCEED();
        }

        TEST(circular_array_methods, at_const_is_noexcept) {
            static_assert(
                noexcept(std::declval<const circular_array<int, 3>&>().at(0))
            );

            SUCCEED();
        }

        TEST(circular_array_methods, at_positive_indices) {
            circular_array values = {10, 20, 30};

            EXPECT_EQ(10, values.at(0));
            EXPECT_EQ(20, values.at(1));
            EXPECT_EQ(30, values.at(2));
        }

        TEST(circular_array_methods, at_wraps_forward) {
            constexpr circular_array values = {10, 20, 30};

            static_assert(10 == values.at(3));
            static_assert(20 == values.at(4));
            static_assert(10 == values.at(6));

            SUCCEED();
        }

        TEST(circular_array_methods, at_wraps_backward) {
            constexpr circular_array values = {10, 20, 30};

            static_assert(30 == values.at(-1));
            static_assert(20 == values.at(-2));
            static_assert(10 == values.at(-3));
            static_assert(30 == values.at(-4));

            SUCCEED();
        }

        TEST(circular_array_methods, at_mut_allows_write) {
            circular_array values = {10, 20, 30};

            values.at(3) = 100;
            EXPECT_EQ(100, values[0]);

            values.at(-1) = 300;
            EXPECT_EQ(300, values[2]);
        }

        TEST(circular_array_methods, at_compile_time) {
            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values.at(0) = 10;
                return 10 == values.at(0) && 2 == values.at(1) && 3 == values.at(2);
            }());

            SUCCEED();
        }

        TEST(circular_array_methods, at_agrees_with_subscript) {
            constexpr circular_array values = {10, 20, 30};

            static_assert(values.at(0)  == values[0]);
            static_assert(values.at(1)  == values[1]);
            static_assert(values.at(-1) == values[-1]);
            static_assert(values.at(4)  == values[4]);

            SUCCEED();
        }

        TEST(circular_array_methods, front_mut_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 1>&>().front()),
                    circular_array<int, 1>::reference>
            );
            static_assert(noexcept(std::declval<circular_array<int, 1>&>().front()));

            static_assert([] -> bool {
                circular_array values = {1};
                values.front() = 10;
                return 10 == values.front() && 10 == values[0];
            }());

            circular_array values = {1};
            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_EQ(&values.back(), &values.front());

            values.front() = 10;
            EXPECT_EQ(10, values[0]);
        }

        TEST(circular_array_methods, front_mut_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().front()),
                    circular_array<int, 3>::reference>
            );
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().front()));

            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values.front() = 10;
                return 10 == values.front() && 10 == values[0] &&
                       2 == values[1] && 3 == values[2];
            }());

            circular_array values = {1, 2, 3};
            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_NE(&values.back(), &values.front());

            values.front() = 10;
            EXPECT_EQ(10, values[0]);
            EXPECT_EQ(2,  values[1]);
            EXPECT_EQ(3,  values[2]);
        }

        TEST(circular_array_methods, front_const_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 1>&>().front()),
                    circular_array<int, 1>::const_reference>
            );
            static_assert(
                std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<const circular_array<int, 1>&>().front())>>
            );
            static_assert(noexcept(std::declval<const circular_array<int, 1>&>().front()));

            constexpr circular_array values = {1};
            static_assert(1 == values.front());
            static_assert(&values[0] == &values.front());
            static_assert(values.front() == values.back());

            EXPECT_EQ(1, values.front());
            EXPECT_EQ(&values[0], &values.front());
            EXPECT_EQ(&values.back(), &values.front());
        }

        TEST(circular_array_methods, front_const_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().front()),
                    circular_array<int, 3>::const_reference>
            );
            static_assert(
                std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<const circular_array<int, 3>&>().front())>>
            );
            static_assert(noexcept(std::declval<const circular_array<int, 3>&>().front()));

            constexpr circular_array values = {1, 2, 3};
            static_assert(1 == values.front());
            static_assert(values.front() == values[0]);

            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_NE(&values.back(), &values.front());
        }

        TEST(circular_array_methods, back_mut_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 1>&>().back()),
                    circular_array<int, 1>::reference>
            );
            static_assert(noexcept(std::declval<circular_array<int, 1>&>().back()));

            static_assert([] -> bool {
                circular_array values = {1};
                values.back() = 10;
                return 10 == values.back() && 10 == values.front();
            }());

            circular_array values = {1};
            EXPECT_EQ(1, values.back());
            EXPECT_EQ(values.data(), &values.back());
            EXPECT_EQ(&values.front(), &values.back());

            values.back() = 10;
            EXPECT_EQ(10, values[0]);
        }

        TEST(circular_array_methods, back_mut_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().back()),
                    circular_array<int, 3>::reference>
            );
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().back()));

            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values.back() = 30;
                return 30 == values.back() && 1 == values[0] &&
                       2 == values[1] && 30 == values[2];
            }());

            circular_array values = {1, 2, 3};
            EXPECT_EQ(3, values.back());
            EXPECT_EQ(&values[2], &values.back());
            EXPECT_EQ(2, &values.back() - &values.front());

            values.back() = 30;
            EXPECT_EQ(1,  values[0]);
            EXPECT_EQ(2,  values[1]);
            EXPECT_EQ(30, values[2]);
        }

        TEST(circular_array_methods, back_const_single_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 1>&>().back()),
                    circular_array<int, 1>::const_reference>
            );
            static_assert(
                std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<const circular_array<int, 1>&>().back())>>
            );
            static_assert(noexcept(std::declval<const circular_array<int, 1>&>().back()));

            constexpr circular_array values = {1};
            static_assert(1 == values.back());

            EXPECT_EQ(1, values.back());
            EXPECT_EQ(values.data(), &values.back());
            EXPECT_EQ(&values.front(), &values.back());
        }

        TEST(circular_array_methods, back_const_multi_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().back()),
                    circular_array<int, 3>::const_reference>
            );
            static_assert(
                std::is_const_v<std::remove_reference_t<
                    decltype(std::declval<const circular_array<int, 3>&>().back())>>
            );
            static_assert(noexcept(std::declval<const circular_array<int, 3>&>().back()));

            constexpr circular_array values = {1, 2, 3};
            static_assert(3 == values.back());
            static_assert(values.back() == values[2]);

            EXPECT_EQ(3, values.back());
            EXPECT_EQ(&values[2], &values.back());
            EXPECT_EQ(2, &values.back() - &values.front());
        }

        TEST(circular_array_methods, data_returns_pointer_to_first_element) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().data()),
                    circular_array<int, 3>::pointer>
            );
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().data()),
                    circular_array<int, 3>::const_pointer>
            );
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().data()));
            static_assert(
                noexcept(std::declval<const circular_array<int, 3>&>().data())
            );

            circular_array values = {1, 2, 3};
            EXPECT_EQ(&values[0], values.data());
            EXPECT_EQ(&values[2], values.data() + 2);
        }

        TEST(circular_array_methods, data_const_is_const_pointer) {
            const circular_array values = {1, 2, 3};
            const int* p = values.data();
            EXPECT_EQ(1, p[0]);
            EXPECT_EQ(2, p[1]);
            EXPECT_EQ(3, p[2]);
        }

        TEST(circular_array_methods, data_points_into_values_field) {
            circular_array<int, 3> values = {1, 2, 3};
            EXPECT_EQ(values.data(), values.values_);
        }

        TEST(circular_array_methods, size_and_max_size_and_empty) {
            static_assert(3 == circular_array<int, 3>{}.size());
            static_assert(3 == circular_array<int, 3>{}.max_size());
            static_assert(!circular_array<int, 3>{}.empty());
            static_assert(1 == circular_array<int, 1>{}.size());
            static_assert(!circular_array<int, 1>{}.empty());
            static_assert(0 == circular_array<int, 0>{}.size());
            static_assert(circular_array<int, 0>{}.empty());

            static_assert(
                noexcept(std::declval<const circular_array<int, 3>&>().size())
            );
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().size()),
                    circular_array<int, 3>::size_type>
            );

            SUCCEED();
        }

        TEST(circular_array_methods, fill_sets_every_element) {
            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values.fill(7);
                return 7 == values[0] && 7 == values[1] && 7 == values[2];
            }());

            circular_array values = {1, 2, 3};
            values.fill(7);
            EXPECT_EQ(7, values[0]);
            EXPECT_EQ(7, values[1]);
            EXPECT_EQ(7, values[2]);

            const int* const before = values.data();
            values.fill(0);
            EXPECT_EQ(before, values.data());
            EXPECT_EQ(0, values[0]);
            EXPECT_EQ(0, values[1]);
            EXPECT_EQ(0, values[2]);
        }

        TEST(circular_array_methods, fill_non_trivial) {
            static constexpr char cstr[] =
                "a string long enough to defeat the small string optimization";

            circular_array<std::string, 3> values;
            values.fill(cstr);

            EXPECT_EQ(cstr, values[0]);
            EXPECT_EQ(cstr, values[1]);
            EXPECT_EQ(cstr, values[2]);
        }

        TEST(circular_array_methods, fill_noexcept_follows_element_assignment) {
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().fill(0)));
            static_assert(
                !noexcept(
                    std::declval<circular_array<std::string, 2>&>().fill(
                        std::declval<const std::string&>()))
            );

            SUCCEED();
        }

        TEST(circular_array_methods, fill_single_element) {
            circular_array<int, 1> values = {0};
            values.fill(42);
            EXPECT_EQ(42, values[0]);
        }

        TEST(circular_array_methods, swap_exchanges_contents_not_addresses) {
            static_assert([] -> bool {
                circular_array first  = {1, 2, 3};
                circular_array second = {4, 5, 6};
                first.swap(second);
                return 4 == first[0] && 6 == first[2] &&
                       1 == second[0] && 3 == second[2];
            }());

            circular_array first  = {1, 2, 3};
            circular_array second = {4, 5, 6};

            const int* const first_data  = first.data();
            const int* const second_data = second.data();

            first.swap(second);

            EXPECT_EQ(4, first[0]);
            EXPECT_EQ(5, first[1]);
            EXPECT_EQ(6, first[2]);
            EXPECT_EQ(1, second[0]);
            EXPECT_EQ(2, second[1]);
            EXPECT_EQ(3, second[2]);
            EXPECT_EQ(first_data,  first.data());
            EXPECT_EQ(second_data, second.data());
        }

        TEST(circular_array_methods, swap_non_trivial) {
            static constexpr char long_a[] =
                "a string long enough to defeat the small string optimization";
            static constexpr char long_b[] =
                "another string long enough to defeat the small string optimization";

            circular_array<std::string, 2> first  = {long_a, "short"};
            circular_array<std::string, 2> second = {long_b, "tiny"};

            first.swap(second);

            EXPECT_EQ(long_b,  first[0]);
            EXPECT_EQ("tiny",  first[1]);
            EXPECT_EQ(long_a,  second[0]);
            EXPECT_EQ("short", second[1]);
        }

        TEST(circular_array_methods, swap_noexcept_follows_element_swap) {
            static_assert(
                noexcept(std::declval<circular_array<int, 3>&>().swap(
                    std::declval<circular_array<int, 3>&>()))
            );
            static_assert(std::is_nothrow_swappable_v<std::string>);
            static_assert(
                noexcept(std::declval<circular_array<std::string, 2>&>().swap(
                    std::declval<circular_array<std::string, 2>&>()))
            );

            SUCCEED();
        }

        TEST(circular_array_methods, sort_no_arg_ascending) {
            circular_array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, sort_no_arg_already_sorted) {
            circular_array values = {1, 2, 3, 4, 5};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, sort_no_arg_reverse_sorted) {
            circular_array values = {5, 4, 3, 2, 1};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, sort_no_arg_single_element) {
            circular_array<int, 1> values = {42};
            values.sort();
            EXPECT_EQ(42, values[0]);
        }

        TEST(circular_array_methods, sort_no_arg_preserves_all_values) {
            circular_array values = {3, 1, 4, 1, 5, 9, 2, 6};
            const auto before = values;
            values.sort();

            // Every element from before should still exist in the sorted array.
            circular_array sorted_before = before;
            sorted_before.sort();
            EXPECT_EQ(sorted_before, values);
        }

        TEST(circular_array_methods, sort_range_overload_partial) {
            circular_array values = {5, 3, 1, 4, 2};
            // Sort only the middle three elements [1..4)
            values.sort(values.begin() + 1, values.begin() + 4);

            EXPECT_EQ(5, values[0]);
            EXPECT_TRUE(std::is_sorted(values.begin() + 1, values.begin() + 4));
            EXPECT_EQ(2, values[4]);
        }

        TEST(circular_array_methods, sort_range_overload_empty_range_no_op) {
            circular_array values = {3, 1, 2};
            const auto before = values;
            values.sort(values.begin(), values.begin());  // empty
            EXPECT_EQ(before, values);
        }

        TEST(circular_array_methods, sort_range_overload_full_range) {
            circular_array values = {3, 1, 2};
            values.sort(values.begin(), values.end());
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, sort_predicate_overload_descending) {
            circular_array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.sort(values.begin(), values.end(), std::greater<int>{});
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end(),
                                       std::greater<int>{}));
        }

        TEST(circular_array_methods, sort_predicate_overload_partial_descending) {
            circular_array values = {5, 3, 1, 4, 2};
            values.sort(values.begin() + 1, values.begin() + 4,
                        std::greater<int>{});

            EXPECT_EQ(5, values[0]);
            EXPECT_TRUE(std::is_sorted(values.begin() + 1, values.begin() + 4,
                                       std::greater<int>{}));
            EXPECT_EQ(2, values[4]);
        }

        TEST(circular_array_methods, sort_predicate_overload_empty_range_no_op) {
            circular_array values = {3, 1, 2};
            const auto before = values;
            values.sort(values.begin(), values.begin(), std::less<int>{});
            EXPECT_EQ(before, values);
        }

        TEST(circular_array_methods, sort_non_trivial_type) {
            circular_array<std::string, 4> values = {"banana", "apple", "date", "cherry"};
            values.sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, stable_sort_no_arg_ascending) {
            circular_array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.stable_sort();
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, stable_sort_no_arg_preserves_relative_order_of_equals) {
            // Use a struct where we can distinguish equal keys.
            struct Item {
                int key;
                int order;
                bool operator==(const Item&) const = default;
                auto operator<=>(const Item&) const = default;
            };

            circular_array<Item, 6> values = {
                Item{2, 0}, Item{1, 1}, Item{2, 2}, Item{1, 3}, Item{2, 4}, Item{1, 5}
            };

            values.stable_sort(values.begin(), values.end(),
                               [](const Item& a, const Item& b) {
                                   return a.key < b.key;
                               });

            // All key=1 items should appear before key=2 items,
            // and within each key group, insertion order must be preserved.
            ASSERT_EQ(1, values[0].key);
            ASSERT_EQ(1, values[1].key);
            ASSERT_EQ(1, values[2].key);
            ASSERT_EQ(2, values[3].key);
            ASSERT_EQ(2, values[4].key);
            ASSERT_EQ(2, values[5].key);

            // Relative order of equals preserved
            EXPECT_EQ(1, values[0].order);
            EXPECT_EQ(3, values[1].order);
            EXPECT_EQ(5, values[2].order);
            EXPECT_EQ(0, values[3].order);
            EXPECT_EQ(2, values[4].order);
            EXPECT_EQ(4, values[5].order);
        }

        TEST(circular_array_methods, stable_sort_range_overload_partial) {
            circular_array values = {5, 3, 1, 4, 2};
            values.stable_sort(values.begin() + 1, values.begin() + 4);

            EXPECT_EQ(5, values[0]);
            EXPECT_TRUE(std::is_sorted(values.begin() + 1, values.begin() + 4));
            EXPECT_EQ(2, values[4]);
        }

        TEST(circular_array_methods, stable_sort_range_overload_empty_range_no_op) {
            circular_array values = {3, 1, 2};
            const auto before = values;
            values.stable_sort(values.begin(), values.begin());
            EXPECT_EQ(before, values);
        }

        TEST(circular_array_methods, stable_sort_range_overload_full_range) {
            circular_array values = {3, 1, 2};
            values.stable_sort(values.begin(), values.end());
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_methods, stable_sort_predicate_overload_descending) {
            circular_array values = {3, 1, 4, 1, 5, 9, 2, 6};
            values.stable_sort(values.begin(), values.end(), std::greater<int>{});
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end(),
                                       std::greater<int>{}));
        }

        TEST(circular_array_methods, stable_sort_predicate_overload_empty_range_no_op) {
            circular_array values = {3, 1, 2};
            const auto before = values;
            values.stable_sort(values.begin(), values.begin(), std::less<int>{});
            EXPECT_EQ(before, values);
        }

    } // namespace methods_testing

    // ── Iterator Tests ──────────────────────────────────────────────────────
    namespace iterator_testing {

        // ── Concepts & traits ────────────────────────────────────────────────

        TEST(circular_array_iterator_traits, satisfies_random_access_iterator) {
            static_assert(
                std::random_access_iterator<circular_array<int, 3>::iterator>
            );
            static_assert(
                std::random_access_iterator<circular_array<int, 3>::const_iterator>
            );

            SUCCEED();
        }

        TEST(circular_array_iterator_traits, satisfies_random_access_range) {
            static_assert(
                std::ranges::random_access_range<circular_array<int, 3>>
            );
            static_assert(
                std::ranges::random_access_range<const circular_array<int, 3>>
            );
            static_assert(
                std::ranges::sized_range<circular_array<int, 3>>
            );
            static_assert(
                std::same_as<
                    std::ranges::range_value_t<circular_array<int, 3>>, int>
            );
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().begin()),
                    circular_array<int, 3>::iterator>
            );
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().begin()),
                    circular_array<int, 3>::const_iterator>
            );

            SUCCEED();
        }

        // ── iterator constructors ────────────────────────────────────────────

        TEST(circular_array_iterator_constructors, default_constructed_iterators_are_equal) {
            constexpr circular_array<int, 0>::iterator lhs;
            constexpr circular_array<int, 0>::iterator rhs;
            static_assert(lhs == rhs);

            SUCCEED();
        }

        TEST(circular_array_iterator_constructors, copy_constructed_iterator_equals_original) {
            circular_array values = {1, 2, 3};
            const auto it  = values.begin();
            const auto it2 = it;

            EXPECT_EQ(it, it2);
            EXPECT_EQ(*it, *it2);
        }

        TEST(circular_array_iterator_constructors, move_constructed_iterator_equals_original) {
            circular_array values = {1, 2, 3};
            auto it  = values.begin();
            auto it2 = std::move(it);

            EXPECT_EQ(values.begin(), it2);
        }

        TEST(circular_array_iterator_constructors, copy_assignment) {
            circular_array values = {1, 2, 3};
            auto it1 = values.begin();
            auto it2 = values.end();

            it2 = it1;
            EXPECT_EQ(it1, it2);
        }

        TEST(circular_array_iterator_constructors, move_assignment) {
            circular_array values = {1, 2, 3};
            auto it1 = values.begin();
            auto it2 = values.end();

            it2 = std::move(it1);
            EXPECT_EQ(values.begin(), it2);
        }

        // ── const_iterator constructors ──────────────────────────────────────

        TEST(circular_array_const_iterator_constructors, default_constructed_are_equal) {
            constexpr circular_array<int, 0>::const_iterator lhs;
            constexpr circular_array<int, 0>::const_iterator rhs;
            static_assert(lhs == rhs);

            SUCCEED();
        }

        TEST(circular_array_const_iterator_constructors, converts_from_iterator) {
            circular_array values = {1, 2, 3};
            circular_array<int, 3>::iterator        it = values.begin();
            circular_array<int, 3>::const_iterator cit = it;

            EXPECT_EQ(it, cit);
            EXPECT_EQ(*it, *cit);
        }

        TEST(circular_array_const_iterator_constructors, copy_assignment) {
            circular_array values = {1, 2, 3};
            circular_array<int, 3>::const_iterator cit1 = values.cbegin();
            circular_array<int, 3>::const_iterator cit2 = values.cend();

            cit2 = cit1;
            EXPECT_EQ(cit1, cit2);
        }

        // ── Dereference & arrow ───────────────────────────────────────────────

        TEST(circular_array_iterator_ops, dereference_reads_element) {
            circular_array values = {10, 20, 30};
            auto it = values.begin();

            EXPECT_EQ(10, *it);
            EXPECT_EQ(20, *(it + 1));
            EXPECT_EQ(30, *(it + 2));
        }

        TEST(circular_array_iterator_ops, dereference_write_mutates_array) {
            circular_array values = {10, 20, 30};
            *values.begin() = 99;
            EXPECT_EQ(99, values[0]);
        }

        TEST(circular_array_iterator_ops, arrow_operator) {
            circular_array<std::string, 2> values = {"hello", "world"};
            auto it = values.begin();

            EXPECT_EQ(5u, it->size());
            ++it;
            EXPECT_EQ(5u, it->size());
        }

        TEST(circular_array_iterator_ops, const_arrow_operator) {
            const circular_array<std::string, 2> values = {"hello", "world"};
            auto it = values.begin();

            EXPECT_EQ(5u, it->size());
        }

        // ── Pre/post increment ────────────────────────────────────────────────

        TEST(circular_array_iterator_ops, pre_increment_advances_and_returns_new) {
            circular_array values = {1, 2, 3};
            auto it = values.begin();

            auto& returned = ++it;
            EXPECT_EQ(2, *returned);
            EXPECT_EQ(&it, &returned);  // returns *this
        }

        TEST(circular_array_iterator_ops, post_increment_advances_and_returns_old) {
            circular_array values = {1, 2, 3};
            auto it  = values.begin();
            auto old = it++;

            EXPECT_EQ(1, *old);
            EXPECT_EQ(2, *it);
        }

        TEST(circular_array_iterator_ops, const_pre_increment) {
            circular_array values = {1, 2, 3};
            auto it = values.cbegin();

            ++it;
            EXPECT_EQ(2, *it);
        }

        TEST(circular_array_iterator_ops, const_post_increment) {
            circular_array values = {1, 2, 3};
            auto it  = values.cbegin();
            auto old = it++;

            EXPECT_EQ(1, *old);
            EXPECT_EQ(2, *it);
        }

        // ── Pre/post decrement ────────────────────────────────────────────────

        TEST(circular_array_iterator_ops, pre_decrement_retreats_and_returns_new) {
            circular_array values = {1, 2, 3};
            auto it = values.end();

            auto& returned = --it;
            EXPECT_EQ(3, *returned);
            EXPECT_EQ(&it, &returned);
        }

        TEST(circular_array_iterator_ops, post_decrement_retreats_and_returns_old) {
            circular_array values = {1, 2, 3};
            auto it  = values.end();
            auto old = it--;

            EXPECT_EQ(old, values.end());
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, const_pre_decrement) {
            circular_array values = {1, 2, 3};
            auto it = values.cend();
            --it;
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, const_post_decrement) {
            circular_array values = {1, 2, 3};
            auto it  = values.cend();
            auto old = it--;

            EXPECT_EQ(old, values.cend());
            EXPECT_EQ(3, *it);
        }

        // ── Arithmetic ────────────────────────────────────────────────────────

        TEST(circular_array_iterator_ops, plus_equals) {
            circular_array values = {1, 2, 3};
            auto it = values.begin();
            it += 2;
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, minus_equals) {
            circular_array values = {1, 2, 3};
            auto it = values.end();
            it -= 2;
            EXPECT_EQ(2, *it);
        }

        TEST(circular_array_iterator_ops, plus_n) {
            circular_array values = {1, 2, 3};
            auto it = values.begin() + 2;
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, n_plus_iterator) {
            circular_array values = {1, 2, 3};
            auto it = 2 + values.begin();
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, minus_n) {
            circular_array values = {1, 2, 3};
            auto it = values.end() - 2;
            EXPECT_EQ(2, *it);
        }

        TEST(circular_array_iterator_ops, iterator_minus_iterator) {
            circular_array values = {1, 2, 3};
            EXPECT_EQ(3, values.end() - values.begin());
            EXPECT_EQ(0, values.begin() - values.begin());
            EXPECT_EQ(1, (values.begin() + 1) - values.begin());
        }

        TEST(circular_array_iterator_ops, const_plus_equals) {
            circular_array values = {1, 2, 3};
            auto it = values.cbegin();
            it += 2;
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, const_minus_equals) {
            circular_array values = {1, 2, 3};
            auto it = values.cend();
            it -= 2;
            EXPECT_EQ(2, *it);
        }

        TEST(circular_array_iterator_ops, const_plus_n) {
            circular_array values = {1, 2, 3};
            auto it = values.cbegin() + 2;
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, const_n_plus_iterator) {
            circular_array values = {1, 2, 3};
            auto it = 2 + values.cbegin();
            EXPECT_EQ(3, *it);
        }

        TEST(circular_array_iterator_ops, const_minus_n) {
            circular_array values = {1, 2, 3};
            auto it = values.cend() - 2;
            EXPECT_EQ(2, *it);
        }

        TEST(circular_array_iterator_ops, const_iterator_minus_const_iterator) {
            circular_array values = {1, 2, 3};
            EXPECT_EQ(3, values.cend() - values.cbegin());
            EXPECT_EQ(0, values.cbegin() - values.cbegin());
        }

        TEST(circular_array_iterator_ops, const_iterator_minus_iterator) {
            circular_array values = {1, 2, 3};
            auto it  = values.begin();
            auto cit = values.cend();

            EXPECT_EQ(3, cit - it);
            EXPECT_EQ(-3, it - cit);
        }

        // ── Subscript on iterator ─────────────────────────────────────────────

        TEST(circular_array_iterator_ops, subscript_random_access) {
            circular_array values = {10, 20, 30};
            auto it = values.begin();

            EXPECT_EQ(10, it[0]);
            EXPECT_EQ(20, it[1]);
            EXPECT_EQ(30, it[2]);
        }

        TEST(circular_array_iterator_ops, subscript_from_middle) {
            circular_array values = {10, 20, 30};
            auto it = values.begin() + 1;

            EXPECT_EQ(20, it[0]);
            EXPECT_EQ(30, it[1]);
        }

        TEST(circular_array_iterator_ops, const_subscript) {
            const circular_array values = {10, 20, 30};
            auto it = values.begin();

            EXPECT_EQ(10, it[0]);
            EXPECT_EQ(20, it[1]);
            EXPECT_EQ(30, it[2]);
        }

        // ── Comparison ────────────────────────────────────────────────────────

        TEST(circular_array_iterator_ops, equality_begin_equals_begin) {
            circular_array values = {1, 2, 3};
            EXPECT_EQ(values.begin(), values.begin());
        }

        TEST(circular_array_iterator_ops, equality_begin_not_equals_end) {
            circular_array values = {1, 2, 3};
            EXPECT_NE(values.begin(), values.end());
        }

        TEST(circular_array_iterator_ops, ordering_less_than) {
            circular_array values = {1, 2, 3};
            EXPECT_LT(values.begin(), values.end());
            EXPECT_LT(values.begin(), values.begin() + 1);
        }

        TEST(circular_array_iterator_ops, ordering_greater_than) {
            circular_array values = {1, 2, 3};
            EXPECT_GT(values.end(), values.begin());
        }

        TEST(circular_array_iterator_ops, ordering_less_equal_greater_equal) {
            circular_array values = {1, 2, 3};
            EXPECT_LE(values.begin(), values.begin());
            EXPECT_LE(values.begin(), values.end());
            EXPECT_GE(values.end(), values.end());
            EXPECT_GE(values.end(), values.begin());
        }

        TEST(circular_array_iterator_ops, const_equality_same) {
            circular_array values = {1, 2, 3};
            EXPECT_EQ(values.cbegin(), values.cbegin());
        }

        TEST(circular_array_iterator_ops, const_equality_not_same) {
            circular_array values = {1, 2, 3};
            EXPECT_NE(values.cbegin(), values.cend());
        }

        TEST(circular_array_iterator_ops, cross_comparison_iterator_const_iterator) {
            circular_array values = {1, 2, 3};
            auto  it  = values.begin();
            auto  cit = values.cbegin();

            EXPECT_EQ(it, cit);
            EXPECT_LE(it, cit);
            EXPECT_GE(it, cit);
        }

        // ── Pointer conversion ────────────────────────────────────────────────

        TEST(circular_array_iterator_ops, iterator_converts_to_raw_pointer) {
            circular_array values = {10, 20, 30};
            auto it = values.begin();

            int* ptr = it;
            EXPECT_EQ(10, *ptr);
            EXPECT_EQ(values.data(), ptr);
        }

        TEST(circular_array_iterator_ops, const_iterator_converts_to_const_raw_pointer) {
            const circular_array values = {10, 20, 30};
            auto it = values.begin();

            const int* ptr = it;
            EXPECT_EQ(10, *ptr);
            EXPECT_EQ(values.data(), ptr);
        }

        // ── begin / end / cbegin / cend ──────────────────────────────────────

        TEST(circular_array_iterator_range, begin_and_end_span_the_array) {
            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                *values.begin() = 10;
                return 10 == values[0] && values.end() - values.begin() == 3;
            }());

            const circular_array values = {1, 2, 3};

            EXPECT_EQ(values.data(),     values.begin());
            EXPECT_EQ(values.data() + 3, values.end());
            EXPECT_EQ(3, values.end() - values.begin());
            EXPECT_EQ(1, *values.begin());
            EXPECT_EQ(3, *(values.end() - 1));
        }

        TEST(circular_array_iterator_range, range_for_loop) {
            constexpr circular_array values = {1, 2, 3};

            int sum = 0;
            for (const int value : values) {
                sum += value;
            }

            EXPECT_EQ(6, sum);
        }

        TEST(circular_array_iterator_range, range_for_loop_mutating) {
            circular_array values = {1, 2, 3};
            for (int& v : values) {
                v *= 2;
            }
            EXPECT_EQ(2, values[0]);
            EXPECT_EQ(4, values[1]);
            EXPECT_EQ(6, values[2]);
        }

        TEST(circular_array_iterator_range, cbegin_and_cend_are_const) {
            circular_array values = {1, 2, 3};

            static_assert(
                std::same_as<
                    decltype(values.cbegin()),
                    circular_array<int, 3>::const_iterator>
            );
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<decltype(*values.cbegin())>>
            );

            EXPECT_EQ(values.data(), values.cbegin());
            EXPECT_EQ(3, values.cend() - values.cbegin());
        }

        // ── rbegin / rend / crbegin / crend ──────────────────────────────────

        TEST(circular_array_iterator_range, rbegin_starts_at_last_element) {
            circular_array values = {1, 2, 3};

            EXPECT_EQ(3, *values.rbegin());
            EXPECT_EQ(1, *(values.rend() - 1));
            EXPECT_EQ(3, values.rend() - values.rbegin());
            EXPECT_EQ(&values.back(), &*values.rbegin());
        }

        TEST(circular_array_iterator_range, reverse_range_for_loop) {
            circular_array values = {1, 2, 3};
            std::vector<int> reversed(values.rbegin(), values.rend());

            EXPECT_THAT(reversed, testing::ElementsAre(3, 2, 1));
        }

        TEST(circular_array_iterator_range, rbegin_write_mutates_back) {
            circular_array values = {1, 2, 3};
            *values.rbegin() = 30;

            EXPECT_EQ(1,  values[0]);
            EXPECT_EQ(2,  values[1]);
            EXPECT_EQ(30, values[2]);
        }

        TEST(circular_array_iterator_range, const_rbegin_rend) {
            const circular_array values = {1, 2, 3};

            static_assert(
                std::same_as<
                    decltype(values.rbegin()),
                    circular_array<int, 3>::const_reverse_iterator>
            );

            EXPECT_EQ(3, *values.rbegin());
            EXPECT_EQ(1, *(values.rend() - 1));
        }

        TEST(circular_array_iterator_range, crbegin_and_crend) {
            constexpr circular_array values = {1, 2, 3};

            static_assert(
                std::same_as<
                    decltype(values.crbegin()),
                    circular_array<int, 3>::const_reverse_iterator>
            );
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<decltype(*values.crbegin())>>
            );

            EXPECT_EQ(3, *values.crbegin());
            EXPECT_EQ(1, *(values.crend() - 1));
            EXPECT_EQ(3, values.crend() - values.crbegin());

            EXPECT_THAT(
                (std::vector(values.crbegin(), values.crend())),
                testing::ElementsAre(3, 2, 1)
            );
        }

        // ── Iterator wrap-around arithmetic ──────────────────────────────────

        TEST(circular_array_iterator_wrap, increment_past_end_wraps) {
            // The iterator's _move_ptr wraps the physical pointer.
            // After N increments from begin(), pos_ wraps back to values_[0].
            circular_array values = {10, 20, 30};

            auto it = values.begin();
            it += 3;  // offset == N, pos_ == values_ + N (past the end)
            // Stepping one more wraps the physical pointer back to [0].
            ++it;
            EXPECT_EQ(10, *it);
        }

        TEST(circular_array_iterator_wrap, negative_offset_iterator_arithmetic) {
            circular_array values = {10, 20, 30};

            // begin() has offset 0; subtracting 1 gives offset -1,
            // _move_ptr maps that to values_[N-1] = values_[2].
            auto it = values.begin() - 1;
            EXPECT_EQ(30, *it);

            auto it2 = values.begin() - 2;
            EXPECT_EQ(20, *it2);
        }

        TEST(circular_array_iterator_wrap, subscript_wrap_through_iterator) {
            circular_array values = {10, 20, 30};
            auto it = values.begin();

            EXPECT_EQ(10, it[3]);   // wraps to [0]
            EXPECT_EQ(20, it[4]);   // wraps to [1]
            EXPECT_EQ(30, it[-1]);  // wraps to [2]
        }

        // ── Integration with standard algorithms ──────────────────────────────

        TEST(circular_array_std_algorithms, std_copy_to_vector) {
            const circular_array values = {1, 2, 3};
            std::vector<int> out(3);
            std::copy(values.begin(), values.end(), out.begin());
            EXPECT_THAT(out, testing::ElementsAre(1, 2, 3));
        }

        TEST(circular_array_std_algorithms, std_fill) {
            circular_array<int, 4> values = {};
            std::fill(values.begin(), values.end(), 7);
            EXPECT_THAT(
                (std::vector(values.begin(), values.end())),
                testing::ElementsAre(7, 7, 7, 7)
            );
        }

        TEST(circular_array_std_algorithms, std_find) {
            circular_array values = {10, 20, 30};
            auto it = std::find(values.begin(), values.end(), 20);

            ASSERT_NE(it, values.end());
            EXPECT_EQ(20, *it);
            EXPECT_EQ(values.begin() + 1, it);
        }

        TEST(circular_array_std_algorithms, std_transform) {
            circular_array values = {1, 2, 3};
            std::transform(values.begin(), values.end(), values.begin(),
                           [](int x) { return x * x; });

            EXPECT_EQ(1, values[0]);
            EXPECT_EQ(4, values[1]);
            EXPECT_EQ(9, values[2]);
        }

        TEST(circular_array_std_algorithms, std_sort_via_iterators) {
            circular_array values = {3, 1, 2};
            std::sort(values.begin(), values.end());
            EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
        }

        TEST(circular_array_std_algorithms, ranges_for_each) {
            circular_array values = {1, 2, 3};
            int sum = 0;
            std::ranges::for_each(values, [&sum](int x) { sum += x; });
            EXPECT_EQ(6, sum);
        }

        TEST(circular_array_std_algorithms, ranges_sort) {
            circular_array values = {3, 1, 2};
            std::ranges::sort(values);
            EXPECT_TRUE(std::ranges::is_sorted(values));
        }

        TEST(circular_array_std_algorithms, ranges_reverse) {
            circular_array values = {1, 2, 3};
            std::ranges::reverse(values);
            EXPECT_EQ(3, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(1, values[2]);
        }
    } // namespace iterator_testing
} // namespace collections::circular_array_testing