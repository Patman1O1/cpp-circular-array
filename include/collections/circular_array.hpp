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
        // ── Forward Declarations ──────────────────────────────────────────────────────────────────────
        class iterator;

        class const_iterator;

        // ── Aliases ───────────────────────────────────────────────────────────────────────────────────
        using value_type = T;

        using size_type = std::size_t;

        using difference_type = std::ptrdiff_t;

        using pointer = value_type*;

        using const_pointer = const value_type*;

        using reference = value_type&;

        using const_reference = const value_type&;

        using reverse_iterator = std::reverse_iterator<iterator>;

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // ── error ───────────────────────────────────────────────────────────────────────────────
        enum class error : std::uint8_t { out_of_range };

        // ── Fields ──────────────────────────────────────────────────────────────────────────────
        value_type values_[N];

    private:
        // ── Methods ─────────────────────────────────────────────────────────────────────────────
        [[gnu::always_inline]] static constexpr auto _move_ptr(
            pointer ptr,
            const difference_type n
        ) noexcept -> pointer { return ptr[n % N]; }

        [[gnu::always_inline]] static constexpr auto _wrap_index(pointer ptr,
                                                                 const difference_type i)
            noexcept -> difference_type { return ptr[((i % N) + N) % N]; }

        template<std::predicate<bool, value_type, value_type> Predicate>
        constexpr void _sort(iterator first, iterator last, Predicate pred) {
            std::sort(first, last, pred);
        }

        template<std::predicate<bool, value_type, value_type> Predicate>
        constexpr void _stable_sort(iterator first, iterator last, Predicate pred) {
            std::stable_sort(first, last, pred);
        }

    public:
        // ── iterator ────────────────────────────────────────────────────────────────────────────
        class iterator {
        public:
            // ── Aliases ─────────────────────────────────────────────────────────────────────────
            using iterator_category = std::random_access_iterator_tag;

            using iterator_concept = std::contiguous_iterator_tag;

            using value_type = T;

            using element_type = T;

            using difference_type = std::ptrdiff_t;

            using size_type = std::size_t;

            using pointer = value_type*;

            using reference = value_type&;

            using const_pointer = const value_type*;

            using const_reference = const value_type&;

        private:
            // ── Friends ─────────────────────────────────────────────────────────────────────────
            friend class circular_array;

            friend class const_iterator;

            // ── Fields ──────────────────────────────────────────────────────────────────────────
            pointer ptr_;

        public:
            // ── Constructors ────────────────────────────────────────────────────────────────────
            constexpr iterator() noexcept : ptr_(nullptr) {}

            explicit constexpr iterator(const pointer ptr) noexcept : ptr_(ptr) {}

            constexpr iterator(const iterator&) noexcept = default;

            constexpr iterator(iterator&&) noexcept = default;

            // ── Overloaded Operators ────────────────────────────────────────────────────────────
            constexpr auto operator=(const iterator&) noexcept -> iterator& = default;

            constexpr auto operator=(iterator&&) noexcept -> iterator& = default;

            [[nodiscard]] constexpr auto operator==(const iterator&)
                const noexcept -> bool = default;

            [[nodiscard]] constexpr auto operator<=>(const iterator&) const noexcept = default;

            [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
                return *this->ptr_;
            }

            [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
                return this->ptr_;
            }

            constexpr auto operator++() noexcept -> iterator& {
                this->ptr_ = _move_ptr(this->ptr_, 1);
                return *this;
            }

            constexpr auto operator++(int) noexcept -> iterator {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr auto operator+=(const difference_type n) noexcept -> iterator& {
                this->ptr_ = _move_ptr(this->ptr_, n);
                return *this;
            }

            [[nodiscard]] constexpr auto operator+(const difference_type n)
                const noexcept -> iterator { return iterator{_move_ptr(this->ptr_, n)}; }

            friend constexpr auto operator+(const difference_type n, const iterator& it)
                noexcept -> iterator { return _move_ptr(it, n); }

            constexpr auto operator--() noexcept -> iterator& {
                this->ptr_ = _move_ptr(this->ptr_, -1);
                return *this;
            }

            constexpr auto operator--(int) noexcept -> iterator {
                iterator tmp = *this;
                --*this;
                return tmp;
            }

            constexpr auto operator-=(const difference_type n) noexcept -> iterator& {
                this->ptr_ -= n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator-(const difference_type n)
                const noexcept -> iterator { return iterator{_move_ptr(this->ptr_, -n)}; }

            [[nodiscard]] constexpr auto operator-(const iterator& other)
                const noexcept -> difference_type { return this->ptr_ - other.ptr_; }

            [[nodiscard]] constexpr auto operator[](const difference_type n)
                const noexcept -> reference { return *_move_ptr(this->ptr_, n); }
        };

        // ── const_iterator ──────────────────────────────────────────────────────────────────────
        class const_iterator {
        public:
            // ── Aliases ─────────────────────────────────────────────────────────────────────────
            using iterator_category = std::random_access_iterator_tag;

            using iterator_concept = std::contiguous_iterator_tag;

            using value_type = T;

            using element_type = T;

            using difference_type = std::ptrdiff_t;

            using size_type = std::size_t;

            using pointer = const value_type*;

            using reference = const value_type&;

            using const_pointer = const value_type*;

            using const_reference = const value_type&;

        private:
            // ── Friends ─────────────────────────────────────────────────────────────────────────
            friend class circular_array;

            // ── Fields ──────────────────────────────────────────────────────────────────────────
            const_pointer ptr_;

        public:
            // ── Constructors ────────────────────────────────────────────────────────────────────
            constexpr const_iterator() noexcept : ptr_(nullptr) {}

            explicit constexpr const_iterator(const const_pointer ptr) noexcept : ptr_(ptr) {}

            constexpr const_iterator(const const_iterator&) noexcept = default;

            constexpr const_iterator(const_iterator&&) noexcept = default;

            // ── Overloaded Operators ────────────────────────────────────────────────────────────
            constexpr auto operator=(const const_iterator&) noexcept -> const_iterator& = default;

            constexpr auto operator=(const_iterator&&) noexcept -> const_iterator& = default;

            [[nodiscard]] constexpr auto operator==(const const_iterator&)
                const noexcept -> bool = default;

            [[nodiscard]] constexpr auto operator<=>(const const_iterator&)
                const noexcept = default;

            [[nodiscard]] constexpr auto operator==(const iterator& other) const noexcept -> bool {
                return this->ptr_ == other.ptr_;
            }

            [[nodiscard]] constexpr auto operator<=>(const iterator& other) const noexcept {
                return this->ptr_ <=> other.ptr_;
            }

            [[nodiscard]] constexpr auto operator*() const noexcept -> const_reference {
                return *this->ptr_;
            }

            [[nodiscard]] constexpr auto operator->() const noexcept -> const_pointer {
                return this->ptr_;
            }

            constexpr auto operator++() noexcept -> const_iterator& {
                ++this->ptr_;
                return *this;
            }

            constexpr auto operator++(int) noexcept -> const_iterator {
                const_iterator tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr auto operator+=(const difference_type n) noexcept -> const_iterator& {
                this->ptr_ += n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator+(const difference_type n)
                const noexcept -> const_iterator { return const_iterator{this->ptr_ + n}; }

            friend constexpr auto operator+(const difference_type n, const const_iterator& it)
                noexcept -> const_iterator { return it + n; }

            constexpr auto operator--() noexcept -> const_iterator& {
                --this->ptr_;
                return *this;
            }

            constexpr auto operator--(int) noexcept -> const_iterator {
                const_iterator tmp = *this;
                --*this;
                return tmp;
            }

            constexpr auto operator-=(const difference_type n) noexcept -> const_iterator& {
                this->ptr_ -= n;
                return *this;
            }

            [[nodiscard]] constexpr auto operator-(const difference_type n)
                const noexcept -> const_iterator { return const_iterator{this->ptr_ - n}; }

            [[nodiscard]] constexpr auto operator-(const const_iterator& other)
                const noexcept -> difference_type { return this->ptr_ - other.ptr_; }

            [[nodiscard]] constexpr auto operator-(const iterator& other)
                const noexcept -> difference_type { return this->ptr_ - other.ptr_; }

            [[nodiscard]] friend constexpr auto operator-(const iterator& lhs,
                                                          const const_iterator& rhs)
                noexcept -> difference_type { return lhs.ptr_ - rhs.ptr_; }

            [[nodiscard]] constexpr auto operator[](const difference_type n)
                const noexcept -> const_reference { return *(this->ptr_ + n); }

        };

        // ── Overloaded Operators ────────────────────────────────────────────────────────────────
        [[nodiscard]] constexpr auto operator==(const circular_array&) const -> bool = default;

        [[nodiscard]] constexpr auto operator<=>(const circular_array&) const = default;

        [[nodiscard]] constexpr auto operator[](const size_type index) noexcept -> reference {
            return this->values_[index];
        }

        [[nodiscard]] constexpr auto operator[](const size_type index)
            const noexcept -> const_reference { return this->values_[index]; }

        // ── Methods ──────────────────────────────────────────────────────────────────────────────────
        [[nodiscard]] constexpr auto at(const size_type index) -> reference {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range("collections::circular_::at index out of range");
            }
            return this->values_[index];
        }

        [[nodiscard]] constexpr auto at(const size_type index) const -> const_reference {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range("collections::array::at index out of range");
            }
            return this->values_[index];
        }

        [[nodiscard]] constexpr auto front() noexcept -> reference {
            return this->values_[0];
        }

        [[nodiscard]] constexpr auto front() const noexcept -> const_reference {
            return this->values_[0];
        }

        [[nodiscard]] constexpr auto back() noexcept -> reference {
            return this->values_[this->size() - 1];
        }

        [[nodiscard]] constexpr auto back() const noexcept -> const_reference {
            return this->values_[this->size() - 1];
        }

        [[nodiscard]] constexpr auto data() noexcept -> pointer {
            return this->values_;
        }

        [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
            return this->values_;
        }

        [[nodiscard]] constexpr auto begin() noexcept -> iterator {
            return iterator{this->values_};
        }

        [[nodiscard]] constexpr auto end() noexcept -> iterator {
            return iterator{this->values_ + N};
        }

        [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
            return const_iterator{this->values_};
        }

        [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
            return const_iterator{this->values_ + N};
        }

        [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
            return const_iterator{this->values_};
        }

        [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
            return const_iterator{this->values_ + N};
        }

        [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(this->end());
        }

        [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(this->begin());
        }

        [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->end());
        }

        [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->begin());
        }

        [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator {
            return this->rbegin();
        }

        [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator {
            return this->rend();
        }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool { return N == 0; }

        [[nodiscard]] constexpr auto size() const noexcept -> size_type { return N; }

        [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return N; }

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
                 std::predicate<bool, value_type, value_type> Predicate>
        constexpr void sort(Iterator first, Iterator last, Predicate pred) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_sort(first, last, pred);
        }

        constexpr void stable_sort() {
            this->_stable_sort(this->begin(), this->end(), std::less<value_type>{});
        }

        constexpr void stable_sort(iterator first, iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_stable_sort(first, last, std::less<value_type>{});
        }

        template<std::predicate<bool, value_type, value_type> Predicate>
        constexpr void stable_sort(iterator first, iterator last, Predicate pred) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_stable_sort(first, last, pred);
        }
    };

    // ── Deduction Guides ────────────────────────────────────────────────────────────────────────
    template<typename T, typename... U> requires (std::same_as<T, U> && ...)
    circular_array(T, U...) -> circular_array<T, 1 + sizeof...(U)>;

} // namespace collections


#endif // COLLECTIONS_ARRAY_HPP