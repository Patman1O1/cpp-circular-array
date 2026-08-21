#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstdint>

// ISO C++ Includes
#include <algorithm>
#include <concepts>
#include <expected>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace collections {
    template<typename T, std::size_t N>
    class circular_array {
    public:
        // ── Forward Declarations ────────────────────────────────────────────
        class iterator;

        class const_iterator;

        // ── Aliases ─────────────────────────────────────────────────────────
        using value_type = T;

        using size_type = std::size_t;

        using difference_type = std::ptrdiff_t;

        using pointer = value_type*;

        using const_pointer = const value_type*;

        using reference = value_type&;

        using const_reference = const value_type&;

        using reverse_iterator = std::reverse_iterator<iterator>;

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // ── error ───────────────────────────────────────────────────────────
        enum class error : std::uint8_t { out_of_range };

        // ── Fields ──────────────────────────────────────────────────────────
        value_type values_[N];

    private:
        // ── Friends ─────────────────────────────────────────────────────────
        friend class iterator;

        friend class const_iterator;

        // ── Fields ──────────────────────────────────────────────────────────
        static constexpr auto SIGNED_N = static_cast<difference_type>(N);

        // ── Methods ─────────────────────────────────────────────────────────
        [[gnu::always_inline]] [[nodiscard]] static constexpr auto _move_ptr(
            const_pointer start,
            const difference_type displacement
        ) noexcept -> pointer {
            return const_cast<pointer>(
                start + (((displacement % SIGNED_N) + SIGNED_N) % SIGNED_N)
            );
        }

        template<std::predicate<value_type, value_type> Predicate>
        constexpr void _sort(iterator first, iterator last, Predicate pred) {
            std::sort(first, last, pred);
        }

        template<std::predicate<value_type, value_type> Predicate>
        constexpr void _stable_sort(iterator first, iterator last, Predicate pred) {
            std::stable_sort(first, last, pred);
        }

    public:
        // ── iterator ────────────────────────────────────────────────────────
        class iterator {
        public:
            // ── Aliases ─────────────────────────────────────────────────────
            // Required by std::iterator_traits / iterator concepts.
            // iterator_category covers the legacy tag hierarchy;
            // iterator_concept is the C++20 concepts tag (random_access here
            // because the circular buffer is not a contiguous memory range).
            using iterator_category = std::random_access_iterator_tag;

            using iterator_concept = std::random_access_iterator_tag;

            using value_type = T;

            using element_type = T;

            using difference_type = std::ptrdiff_t;

            using pointer = value_type*;

            using reference = value_type&;

        private:
            // ── Friends ─────────────────────────────────────────────────────
            friend class circular_array;

            friend class const_iterator;

            // ── Fields ──────────────────────────────────────────────────────
            const_pointer start_;

            pointer pos_;

            // ── Methods ─────────────────────────────────────────────────────
            [[gnu::always_inline]] [[nodiscard]] constexpr auto _offset()
                const noexcept -> difference_type {
                return this->pos_ - this->start_;
            }

        public:
            // ── Constructors ────────────────────────────────────────────────
            constexpr iterator() noexcept
                : start_(nullptr), pos_(nullptr) {}

            constexpr iterator(
                const_pointer start,
                pointer pos
            ) noexcept : start_(start), pos_(pos) {}

            constexpr iterator(const iterator&) noexcept = default;

            constexpr iterator(iterator&&) noexcept = default;

            // ── Overloaded Operators ────────────────────────────────────────
            constexpr auto operator=(
                const iterator&
            ) noexcept -> iterator& = default;

            constexpr auto operator=(
                iterator&&
            ) noexcept -> iterator& = default;

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator==(
                const iterator& other
            ) const noexcept -> bool { return this->pos_ == other.pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator<=>(
                const iterator& other
            ) const noexcept { return this->pos_ <=> other.pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator*()
                const noexcept -> reference { return *this->pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator->()
                const noexcept -> pointer { return this->pos_; }

            constexpr auto operator++() noexcept -> iterator& {
                ++this->pos_;
                return *this;
            }

            constexpr auto operator++(int) noexcept -> iterator {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr auto operator+=(
                const difference_type n
            ) noexcept -> iterator& {
                this->pos_ += n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator+(const difference_type n)
                const noexcept -> iterator {
                return iterator{this->start_, this->pos_ + n};
            }

            friend constexpr auto operator+(
                const difference_type n,
                const iterator& it
            ) noexcept -> iterator { return it + n; }

            constexpr auto operator--() noexcept -> iterator& {
                --this->pos_;
                return *this;
            }

            constexpr auto operator--(int) noexcept -> iterator {
                iterator tmp = *this;
                --*this;
                return tmp;
            }

            constexpr auto operator-=(
                const difference_type n
            ) noexcept -> iterator& {
                this->pos_ -= n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator-(const difference_type n)
                const noexcept -> iterator {
                return iterator{this->start_, this->pos_ - n};
            }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator-(
                const iterator& other
            ) const noexcept -> difference_type {
                return this->pos_ - other.pos_;
            }

            [[nodiscard]] constexpr auto operator[](const difference_type n)
                const noexcept -> reference {
                return *_move_ptr(this->start_, this->_offset() + n);
            }

            [[nodiscard]] constexpr operator const_iterator() const noexcept {
                return const_iterator{this->start_, this->pos_};
            }

            [[gnu::always_inline]] [[nodiscard]] constexpr operator pointer()
                const noexcept { return this->pos_; }
        };

        // ── const_iterator ──────────────────────────────────────────────────
        class const_iterator {
        public:
            // ── Aliases ─────────────────────────────────────────────────────
            using iterator_category = std::random_access_iterator_tag;

            using iterator_concept = std::random_access_iterator_tag;

            using value_type = T;

            using element_type = T;

            using difference_type = std::ptrdiff_t;

            using pointer = const value_type*;

            using reference = const value_type&;

        private:
            // ── Friends ─────────────────────────────────────────────────────
            friend class circular_array;

            friend class iterator;

            // ── Fields ──────────────────────────────────────────────────────
            const_pointer start_;

            const_pointer pos_;

        public:
            // ── Constructors ────────────────────────────────────────────────
            constexpr const_iterator() noexcept
                : start_(nullptr), pos_(nullptr) {}

            constexpr const_iterator(
                const_pointer start,
                const_pointer pos
            ) noexcept : start_(start), pos_(pos) {}

            constexpr const_iterator(const iterator& it) noexcept
                : start_(it.start_), pos_(it.pos_) {}

            constexpr const_iterator(const const_iterator&) noexcept = default;

            constexpr const_iterator(const_iterator&&) noexcept = default;

            // ── Overloaded Operators ────────────────────────────────────────
            constexpr auto operator=(
                const const_iterator&
            ) noexcept -> const_iterator& = default;

            constexpr auto operator=(
                const_iterator&&
            ) noexcept -> const_iterator& = default;

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator==(
                const const_iterator& other
            ) const noexcept -> bool { return this->pos_ == other.pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator<=>(
                const const_iterator& other
            ) const noexcept { return this->pos_ <=> other.pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator==(
                const iterator& other
            ) const noexcept -> bool { return this->pos_ == other.pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator<=>(
                const iterator& other
            ) const noexcept { return this->pos_ <=> other.pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator*()
                const noexcept -> reference { return *this->pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator->()
                const noexcept -> pointer { return this->pos_; }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto _offset()
                const noexcept -> difference_type {
                return this->pos_ - this->start_;
            }

            constexpr auto operator++() noexcept -> const_iterator& {
                ++this->pos_;
                return *this;
            }

            constexpr auto operator++(int) noexcept -> const_iterator {
                const_iterator tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr auto operator+=(
                const difference_type n
            ) noexcept -> const_iterator& {
                this->pos_ += n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator+(const difference_type n)
                const noexcept -> const_iterator {
                return const_iterator{this->start_, this->pos_ + n};
            }

            friend constexpr auto operator+(
                const difference_type n,
                const const_iterator& it
            ) noexcept -> const_iterator { return it + n; }

            constexpr auto operator--() noexcept -> const_iterator& {
                --this->pos_;
                return *this;
            }

            constexpr auto operator--(int) noexcept -> const_iterator {
                const_iterator tmp = *this;
                --*this;
                return tmp;
            }

            constexpr auto operator-=(
                const difference_type n
            ) noexcept -> const_iterator& {
                this->pos_ -= n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator-(const difference_type n)
                const noexcept -> const_iterator {
                return const_iterator{this->start_, this->pos_ - n};
            }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator-(
                const const_iterator& other
            ) const noexcept -> difference_type {
                return this->pos_ - other.pos_;
            }

            [[gnu::always_inline]] [[nodiscard]] constexpr auto operator-(
                const iterator& other
            ) const noexcept -> difference_type {
                return this->pos_ - other.pos_;
            }

            [[gnu::always_inline]] [[nodiscard]] friend constexpr auto operator-(
                const iterator& lhs,
                const const_iterator& rhs
            ) noexcept -> difference_type { return lhs.pos_ - rhs.pos_; }

            [[nodiscard]] constexpr auto operator[](const difference_type n)
                const noexcept -> reference {
                return *_move_ptr(this->start_, this->_offset() + n);
            }

            [[gnu::always_inline]] [[nodiscard]] constexpr operator const_pointer()
                const noexcept { return this->pos_; }
        };

        // ── Overloaded Operators ────────────────────────────────────────────
        [[nodiscard]] constexpr auto operator==(const circular_array&)
            const -> bool = default;

        [[nodiscard]] constexpr auto operator<=>(const circular_array&)
            const = default;

        [[nodiscard]] constexpr auto operator[](const difference_type index)
            noexcept -> reference {
            return *_move_ptr(this->values_, index);
        }

        [[nodiscard]] constexpr auto operator[](const difference_type index)
            const noexcept -> const_reference {
            return *_move_ptr(this->values_, index);
        }

        // ── Methods ─────────────────────────────────────────────────────────
        [[nodiscard]] constexpr auto at(const difference_type index)
            noexcept -> reference {
            return *_move_ptr(this->values_, index);
        }

        [[nodiscard]] constexpr auto at(const difference_type index)
            const noexcept -> const_reference {
            return *_move_ptr(this->values_, index);
        }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto front()
            noexcept -> reference { return this->values_[0]; }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto front()
            const noexcept -> const_reference { return this->values_[0]; }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto back()
            noexcept -> reference { return this->values_[this->size() - 1]; }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto back()
            const noexcept -> const_reference {
            return this->values_[this->size() - 1];
        }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto data()
            noexcept -> pointer { return this->values_; }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto data()
            const noexcept -> const_pointer { return this->values_; }

        [[nodiscard]] constexpr auto begin() noexcept -> iterator {
            return iterator{this->values_, this->values_};
        }

        [[nodiscard]] constexpr auto end() noexcept -> iterator {
            return iterator{this->values_, this->values_ + N};
        }

        [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
            return const_iterator{this->values_, this->values_};
        }

        [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
            return const_iterator{this->values_, this->values_ + N};
        }

        [[nodiscard]] constexpr auto cbegin()
            const noexcept -> const_iterator {
            return const_iterator{this->values_, this->values_};
        }

        [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
            return const_iterator{this->values_, this->values_ + N};
        }

        [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(this->end());
        }

        [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(this->begin());
        }

        [[nodiscard]] constexpr auto rbegin()
            const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->end());
        }

        [[nodiscard]] constexpr auto rend()
            const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->begin());
        }

        [[nodiscard]] constexpr auto crbegin()
            const noexcept -> const_reverse_iterator { return this->rbegin(); }

        [[nodiscard]] constexpr auto crend()
            const noexcept -> const_reverse_iterator { return this->rend(); }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto empty()
            const noexcept -> bool { return N == 0; }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto size()
            const noexcept -> size_type { return N; }

        [[gnu::always_inline]] [[nodiscard]] constexpr auto max_size()
            const noexcept -> size_type { return N; }

        constexpr void fill(const_reference value)
            noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            std::fill(this->begin(), this->end(), value);
        }

        constexpr void swap(circular_array& other)
            noexcept(std::is_nothrow_swappable_v<value_type>) {
            std::swap(this->values_, other.values_);
        }

        constexpr void sort() {
            this->_sort(this->begin(), this->end(), std::less<value_type>{});
        }

        template<std::contiguous_iterator Iterator = iterator>
        constexpr void sort(Iterator first, Iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_sort(first, last, std::less<value_type>{});
        }

        template<std::contiguous_iterator Iterator = iterator,
                 std::predicate<value_type, value_type> Predicate>
        constexpr void sort(Iterator first, Iterator last, Predicate pred) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_sort(first, last, pred);
        }

        constexpr void stable_sort() {
            this->_stable_sort(
                this->begin(),
                this->end(),
                std::less<value_type>{}
            );
        }

        constexpr void stable_sort(iterator first, iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_stable_sort(first, last, std::less<value_type>{});
        }

        template<std::predicate<value_type, value_type> Predicate>
        constexpr void stable_sort(
            iterator first,
            iterator last,
            Predicate pred
        ) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_stable_sort(first, last, pred);
        }
    };

    // ── Deduction Guides ────────────────────────────────────────────────────
    template<typename T, typename... U> requires (std::same_as<T, U> && ...)
    circular_array(T, U...) -> circular_array<T, 1 + sizeof...(U)>;

} // namespace collections

#endif // #ifndef COLLECTIONS_ARRAY_HPP
