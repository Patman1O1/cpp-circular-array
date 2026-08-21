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
    namespace aggregate_tests {
        TEST(circular_array_aggregate, is_aggregate) {
            static_assert(std::is_aggregate_v<circular_array<int, 3>>);
            static_assert(std::is_aggregate_v<circular_array<std::string, 2>>);

            SUCCEED();
        }

        TEST(
            circular_array_aggregate, 
            brace_init_checks_arity_at_compile_time
        ) {
            static_assert(
                brace_initializable<
                    circular_array<int, 3>,
                    int,
                    int,
                    int
                >
            );

            static_assert(
                    brace_initializable<circular_array<int, 3>,
                    int,
                    int
                >
            );

            static_assert(brace_initializable<circular_array<int, 3>, int>);
            static_assert(brace_initializable<circular_array<int, 3>>);
            static_assert(!brace_initializable<circular_array<int, 3>, int, int, int, int>);

            static_assert(!brace_initializable<circular_array<int, 2>, int, std::string>);

            SUCCEED();
        }

        TEST(circular_array_aggregate, brace_init_rejects_narrowing) {
            static_assert(!brace_initializable<circular_array<int, 2>, double, double>);
            static_assert(!brace_initializable<circular_array<int, 2>, int, long long>);
            static_assert(!brace_initializable<circular_array<char, 2>, int, int>);

            static_assert(!brace_initializable<circular_array<double, 2>, int, int>);

            SUCCEED();
        }

        TEST(circular_array_aggregate, brace_init_value_initializes_the_unsupplied_tail) {
            constexpr circular_array<int, 3> values = {7};
            static_assert(7 == values[0]);
            static_assert(0 == values[1]);
            static_assert(0 == values[2]);

            SUCCEED();
        }

        TEST(circular_array_aggregate, has_no_implicit_conversion_from_a_single_element) {
            static_assert(!std::is_convertible_v<int, circular_array<int, 3>>);

            SUCCEED();
        }

        TEST(circular_array_aggregate, brace_init_moves_rvalue_elements) {
            std::string first = "a string long enough to defeat the small string optimization";
            std::string second = "another string long enough to defeat the small string optimization";

            const circular_array values = {std::move(first), std::move(second)};

            EXPECT_EQ(values[0], "a string long enough to defeat the small string optimization");
            EXPECT_EQ(values[1], "another string long enough to defeat the small string optimization");

            EXPECT_NE(first, "a string long enough to defeat the small string optimization");
            EXPECT_NE(second, "another string long enough to defeat the small string optimization");
        }

        TEST(circular_array_aggregate, special_members_are_implicit_and_trivial) {
            static_assert(std::is_trivially_default_constructible_v<circular_array<int, 3>>);
            static_assert(std::is_trivially_copy_constructible_v<circular_array<int, 3>>);
            static_assert(std::is_trivially_move_constructible_v<circular_array<int, 3>>);
            static_assert(std::is_trivially_copy_assignable_v<circular_array<int, 3>>);
            static_assert(std::is_trivially_move_assignable_v<circular_array<int, 3>>);
            static_assert(std::is_trivially_destructible_v<circular_array<int, 3>>);
            static_assert(std::is_trivially_copyable_v<circular_array<int, 3>>);

            // Layout must match the bare C array it wraps.
            static_assert(std::is_standard_layout_v<circular_array<int, 3>>);
            static_assert(sizeof(circular_array<int, 3>) == sizeof(int[3]));
            static_assert(alignof(circular_array<int, 3>) == alignof(int[3]));

            // A non-trivial value_type must propagate, not be papered over.
            static_assert(!std::is_trivially_copyable_v<circular_array<std::string, 2>>);
            static_assert(std::is_copy_constructible_v<circular_array<std::string, 2>>);
            static_assert(std::is_move_constructible_v<circular_array<std::string, 2>>);
            static_assert(std::is_copy_assignable_v<circular_array<std::string, 2>>);
            static_assert(std::is_move_assignable_v<circular_array<std::string, 2>>);

            SUCCEED();
        }

        TEST(circular_array_aggregate, copy_from_a_non_const_lvalue_is_a_trivial_copy) {
            circular_array source = {1, 2, 3};
            circular_array<int, 3> copy = source;

            EXPECT_EQ(1, copy[0]);
            EXPECT_EQ(2, copy[1]);
            EXPECT_EQ(3, copy[2]);
            EXPECT_NE(copy.data(), source.data());
        }

        TEST(circular_array_aggregate, copy_assignment_is_not_deleted) {
            constexpr circular_array source = {1, 2, 3};
            circular_array<int, 3> target = {};

            target = source;

            EXPECT_EQ(1, target[0]);
            EXPECT_EQ(2, target[1]);
            EXPECT_EQ(3, target[2]);
        }

        TEST(circular_array_aggregate, deduces_its_template_arguments) {
            circular_array values = {1, 2, 3};
            static_assert(std::same_as<decltype(values), circular_array<int, 3>>);

            [[maybe_unused]] circular_array singleton = {1};
            static_assert(std::same_as<decltype(singleton), circular_array<int, 1>>);

            static_assert(deducible<int, int, int>);
            static_assert(!deducible<int, double>);

            EXPECT_EQ(1, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(3, values[2]);
        }

        TEST(circular_array_aggregate, is_structural) {
            static_assert(std::is_structural_v<circular_array<int, 3>>); // Ignore this error, this is valid in C++26

            SUCCEED();
        }
    } // namespace aggregate_tests

    // ── Overloaded Operators Tests ──────────────────────────────────────────
    namespace overloaded_operators_tests {
        TEST(circular_array_operators, random_access_mut_overload_returns_mut_ref) {
            static_assert(std::same_as<decltype(std::declval<circular_array<int, 3>&>()[0]),
                          circular_array<int, 3>::reference>);
            static_assert(
                !std::is_const_v<std::remove_reference_t<decltype(std::declval<circular_array<int, 3>&>()[0])>>
            );

            SUCCEED();
        }

        TEST(circular_array_operators, random_access_mut_overload_is_noexcept) {
            static_assert(noexcept(std::declval<circular_array<int, 3>&>()[0]));

            SUCCEED();
        }

        TEST(circular_array_operators, random_access_mut_overload_is_usable_in_constant_expressions) {
            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values[0] = 10;
                return 10 == values[0] && 2 == values[1] && 3 == values[2];
            }());

            SUCCEED();
        }

        TEST(circular_array_operators, random_access_mut_overload_aliases_storage) {
            circular_array values = {1, 2, 3};

            const int& first = values[0];
            int& second = values[1];

            EXPECT_EQ(1, &second - &first);

            second = 20;
            EXPECT_EQ(20, values[1]);
        }

        TEST(circular_array_operators, random_access_const_overload_returns_const_ref) {
            static_assert(std::same_as<decltype(std::declval<const circular_array<int, 3>&>()[0]),
                          circular_array<int, 3>::const_reference>);
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<decltype(std::declval<const circular_array<int, 3>&>()[0])>
                >
            );

            SUCCEED();
        }

        TEST(circular_array_operators, random_access_const_overload_is_noexcept) {
            static_assert(noexcept(std::declval<const circular_array<int, 3>&>()[0]));

            SUCCEED();
        }

        TEST(circular_array_operators, random_access_const_overload_is_usable_in_constant_expressions) {
            constexpr circular_array values = {1, 2, 3};
            static_assert(1 == values[0]);
            static_assert(2 == values[1]);
            static_assert(3 == values[2]);

            SUCCEED();
        }

        TEST(circular_array_operators, random_access_const_overload_aliases_storage) {
            constexpr circular_array values = {1, 2, 3};

            const int& first = values[0];
            const int& second = values[1];

            EXPECT_EQ(1, &second - &first);
        }

        TEST(circular_array_operators, random_access_const_overload_does_not_copy_a_non_trivial_element) {
            const circular_array values = {std::string("alpha"), std::string("beta")};

            EXPECT_EQ("alpha", values[0]);

            EXPECT_EQ(values.data(), &values[0]);
        }
    } // namespace overloaded_operators_tests

    // ── Method Tests ────────────────────────────────────────────────────────
    namespace methods_tests {
        TEST(circular_array_methods, at_mut_overload_returns_mut_ref) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().at(0)), circular_array<int, 3>::reference
                >
            );

            static_assert(
                !std::is_const_v<
                    std::remove_reference_t<decltype(std::declval<circular_array<int, 3>&>().at(0))>
                >
            );

            SUCCEED();
        }

        TEST(circular_array_methods, at_mut_overload_is_noexcept) {            
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().at(0)));
        }

        TEST(circular_array_methods, at_const_overload_returns_const_ref) {
            static_assert(std::same_as<decltype(std::declval<const circular_array<int, 3>&>().at(0)),
                                       circular_array<int, 3>::const_reference>);
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<decltype(std::declval<const circular_array<int, 3>&>().at(0))>
                >
            );

            SUCCEED();
        }

        TEST(circular_array_methods, at_const_overload_is_noexcept) {
            static_assert(noexcept(std::declval<const circular_array<int, 3>&>().at(0)));

            SUCCEED();
        }

        TEST(circular_array_methods, front_mut_overload_single_element_array) {
            static_assert(std::same_as<decltype(std::declval<circular_array<int, 1>&>().front()), circular_array<int, 1>::reference>);
            static_assert(noexcept(std::declval<circular_array<int, 1>&>().front()));

            static_assert([] -> bool {
                circular_array values = {1};
                values.front() = 10;
                return 10 == values.front() && 10 == values[0];
            }());

            circular_array values = {1};
            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());

            // With one element, front and back are the same object.
            EXPECT_EQ(&values.back(), &values.front());

            values.front() = 10;
            EXPECT_EQ(10, values[0]);
        }

        TEST(circular_array_methods, front_mut_overload_multi_element_array) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 3>&>().front()), circular_array<int, 3>::reference
                >
            );
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().front()));

            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values.front() = 10;
                return 10 == values.front() && 10 == values[0] && 2 == values[1] && 3 == values[2];
            }());

            circular_array values = {1, 2, 3};
            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_NE(&values.back(), &values.front());

            values.front() = 10;
            EXPECT_EQ(10, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(3, values[2]);
        }

        TEST(circular_array_methods, front_const_overload_single_element_array) {
            static_assert(std::same_as<decltype(std::declval<const circular_array<int, 1>&>().front()),
                                       circular_array<int, 1>::const_reference>);
            static_assert(
                std::is_const_v<
                    std::remove_reference_t<decltype(std::declval<const circular_array<int, 1>&>().front())>
                >
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

        TEST(circular_array_methods, front_const_overload_multi_element_array) {
            static_assert(
                std::same_as<
                    decltype(std::declval<const circular_array<int, 3>&>().front()),
                    circular_array<int, 3>::const_reference
                >
            );

            static_assert(
                std::is_const_v<
                    std::remove_reference_t<decltype(std::declval<const circular_array<int, 3>&>().front())>
                >
            );

            static_assert(noexcept(std::declval<const circular_array<int, 3>&>().front()));

            constexpr circular_array values = {1, 2, 3};

            static_assert(1 == values.front());
            static_assert(values.front() == values[0]);

            EXPECT_EQ(1, values.front());
            EXPECT_EQ(values.data(), &values.front());
            EXPECT_NE(&values.back(), &values.front());
        }

        TEST(circular_array_methods, back_mut_overload_single_element_array) {
            static_assert(
                std::same_as<
                    decltype(std::declval<circular_array<int, 1>&>().back()), circular_array<int, 1>::reference
                >
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

        TEST(circular_array_methods, back_mut_overload_multi_element_array) {
            static_assert(
                std::same_as<decltype(std::declval<circular_array<int, 3>&>().back()), circular_array<int, 3>::reference>
            );
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().back()));

            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                values.back() = 30;
                return 30 == values.back() && 1 == values[0] && 2 == values[1] && 30 == values[2];
            }());

            circular_array values = {1, 2, 3};

            EXPECT_EQ(3, values.back());
            EXPECT_EQ(&values[2], &values.back());
            EXPECT_EQ(2, &values.back() - &values.front());

            values.back() = 30;
            EXPECT_EQ(1, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(30, values[2]);
        }

        TEST(circular_array_methods, back_const_overload_single_element_array) {
            static_assert(
                std::same_as<
                    decltype(
                        std::declval<const circular_array<int, 1>&>().back()
                    ), circular_array<int, 1>::const_reference
                >
            );
            static_assert(std::is_const_v<
                std::remove_reference_t<decltype(std::declval<const circular_array<int, 1>&>().back())>>);
            static_assert(noexcept(std::declval<const circular_array<int, 1>&>().back()));

            constexpr circular_array values = {1};
            static_assert(1 == values.back());

            EXPECT_EQ(1, values.back());
            EXPECT_EQ(values.data(), &values.back());
            EXPECT_EQ(&values.front(), &values.back());
        }

        TEST(circular_array_methods, back_const_overload_multi_element_array) {
            static_assert(std::same_as<decltype(std::declval<const circular_array<int, 3>&>().back()),
                                       circular_array<int, 3>::const_reference>);
            static_assert(std::is_const_v<
                std::remove_reference_t<decltype(std::declval<const circular_array<int, 3>&>().back())>>);
            static_assert(noexcept(std::declval<const circular_array<int, 3>&>().back()));

            constexpr circular_array values = {1, 2, 3};
            static_assert(3 == values.back());
            static_assert(values.back() == values[2]);

            EXPECT_EQ(3, values.back());
            EXPECT_EQ(&values[2], &values.back());
            EXPECT_EQ(2, &values.back() - &values.front());
        }

        TEST(circular_array_methods, data_returns_the_address_of_the_first_element) {
            static_assert(
                std::same_as<decltype(std::declval<circular_array<int, 3>&>().data()), circular_array<int, 3>::pointer>
            );

            static_assert(
                std::same_as<decltype(std::declval<const circular_array<int, 3>&>().data()),
                circular_array<int, 3>::const_pointer>
            );

            static_assert(noexcept(std::declval<circular_array<int, 3>&>().data()));

            circular_array values = {1, 2, 3};
            EXPECT_EQ(values.data(), values.data());
            EXPECT_EQ(&values[2], values.data() + 2);
        }

        TEST(circular_array_methods, size_and_max_size_and_empty_are_compile_time_constants) {
            static_assert(3 == circular_array<int, 3>{}.size());
            static_assert(3 == circular_array<int, 3>{}.max_size());
            static_assert(!circular_array<int, 3>{}.empty());
            static_assert(1 == circular_array<int, 1>{}.size());
            static_assert(!circular_array<int, 1>{}.empty());

            static_assert(noexcept(std::declval<const circular_array<int, 3>&>().size()));
            static_assert(std::same_as<decltype(std::declval<const circular_array<int, 3>&>().size()),
                                       circular_array<int, 3>::size_type>);

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

        TEST(circular_array_methods, fill_noexcept_follows_the_element_assignment) {
            static_assert(noexcept(std::declval<circular_array<int, 3>&>().fill(0)));

            static_assert(
                !noexcept(
                    std::declval<circular_array<std::string, 2>&>().fill(std::declval<const std::string&>())
                )
            );

            SUCCEED();
        }

        TEST(circular_array_methods, swap_exchanges_contents_not_addresses) {
            static_assert([] -> bool {
                circular_array first = {1, 2, 3};
                circular_array second = {4, 5, 6};
                first.swap(second);
                return 4 == first[0] && 6 == first[2] && 1 == second[0] && 3 == second[2];
            }());

            circular_array first = {1, 2, 3};
            circular_array second = {4, 5, 6};

            const int* const first_data = first.data();
            const int* const second_data = second.data();

            first.swap(second);

            EXPECT_EQ(4, first[0]);
            EXPECT_EQ(5, first[1]);
            EXPECT_EQ(6, first[2]);

            EXPECT_EQ(1, second_data[0]);
            EXPECT_EQ(2, second_data[1]);
            EXPECT_EQ(3, second_data[2]);

            EXPECT_EQ(first_data, first.data());
            EXPECT_EQ(second_data, second.data());
        }

        TEST(circular_array_methods, swap_noexcept_follows_the_element_swap) {
            static_assert(
                noexcept(std::declval<circular_array<int, 3>&>().swap(std::declval<circular_array<int, 3>&>()))
            );

            static_assert(std::is_nothrow_swappable_v<std::string>);
            static_assert(
                noexcept(
                    std::declval<circular_array<std::string, 2>&>().swap(std::declval<circular_array<std::string, 2>&>())
                )
            );

            SUCCEED();
        }
    } // namespace methods_tests

    // ── Iterator Tests ──────────────────────────────────────────────────────
    namespace iterator_tests {
        TEST(circular_array_iterators, satisfies_random_access_range) {
            static_assert(std::ranges::random_access_range<circular_array<int, 3>>);

            static_assert(std::ranges::random_access_range<const circular_array<int, 3>>);
            static_assert(std::ranges::sized_range<circular_array<int, 3>>);

            static_assert(std::same_as<std::ranges::range_value_t<circular_array<int, 3>>, int>);
            static_assert(
                std::same_as<decltype(std::declval<circular_array<int, 3>&>().begin()), circular_array<int, 3>::iterator>
            );
            static_assert(
                std::same_as<decltype(std::declval<const circular_array<int, 3>&>().begin()),
                circular_array<int, 3>::const_iterator>
            );

            SUCCEED();
        }

        TEST(circular_array_iterators, begin_and_end_span_the_array) {
            static_assert([] -> bool {
                circular_array values = {1, 2, 3};
                *values.begin() = 10;
                return 10 == values[0] && values.end() - values.begin() == 3;
            }());

            circular_array values = {1, 2, 3};

            EXPECT_EQ(values.data(), values.begin());
            EXPECT_EQ(values.data() + 3, values.end());
            EXPECT_EQ(3, values.end() - values.begin());
            EXPECT_EQ(1, *values.begin());
            EXPECT_EQ(3, *(values.end() - 1));
        }

        TEST(circular_array_iterators, a_const_array_is_iterable_with_a_range_for) {
            constexpr circular_array values = {1, 2, 3};

            int sum = 0;
            for (const int value : values) {
                sum += value;
            }

            EXPECT_EQ(6, sum);
            EXPECT_EQ(values.data(), values.begin());
            EXPECT_EQ(3, values.end() - values.begin());
        }

        TEST(circular_array_iterators, cbegin_and_cend_are_const_on_a_mutable_array) {
            circular_array values = {1, 2, 3};

            static_assert(std::same_as<decltype(values.cbegin()), circular_array<int, 3>::const_iterator>);
            static_assert(std::is_const_v<std::remove_reference_t<decltype(*values.cbegin())>>);

            EXPECT_EQ(values.data(), values.cbegin());
            EXPECT_EQ(3, values.cend() - values.cbegin());
        }

        TEST(circular_array_iterators, rbegin_starts_at_the_last_element) {
            circular_array values = {1, 2, 3};

            EXPECT_EQ(3, *values.rbegin());
            EXPECT_EQ(1, *(values.rend() - 1));
            EXPECT_EQ(3, values.rend() - values.rbegin());
            EXPECT_EQ(&values.back(), &*values.rbegin());

            const std::vector<int> span{values.begin(), values.end()};
            EXPECT_EQ(1, span[0]);
            EXPECT_EQ(2, span[1]);
            EXPECT_EQ(3, span[2]);

            *values.rbegin() = 30;
            EXPECT_EQ(1, values[0]);
            EXPECT_EQ(2, values[1]);
            EXPECT_EQ(30, values[2]);
        }

        TEST(circular_array_iterators, crbegin_and_crend_walk_backwards_constly) {
            constexpr circular_array values = {1, 2, 3};

            static_assert(
                std::same_as<decltype(values.crbegin()), circular_array<int, 3>::const_reverse_iterator>
            );
            static_assert(std::is_const_v<std::remove_reference_t<decltype(*values.crbegin())>>);

            EXPECT_EQ(3, *values.crbegin());
            EXPECT_EQ(1, *(values.crend() - 1));
            EXPECT_EQ(3, values.crend() - values.crbegin());

            const std::vector<int> span{values.crbegin(), values.crend()};
            EXPECT_EQ(3, span[0]);
            EXPECT_EQ(2, span[1]);
            EXPECT_EQ(1, span[2]);
            EXPECT_THAT(
                (std::vector(values.crbegin(), values.crend())), testing::ElementsAre(3, 2, 1)
            );
        }
    } // namespace iterator_tests
} // namespace collections::circular_array_testing
