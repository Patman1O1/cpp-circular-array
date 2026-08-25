#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstring>

// ISO C++ Includes
#include <algorithm>
#include <iterator>

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

        // ── Fields ──────────────────────────────────────────────────────────
        value_type values_[N];

    private:
        // ── Friends ─────────────────────────────────────────────────────────
        friend class iterator;

        friend class const_iterator;

        // ── Fields ──────────────────────────────────────────────────────────
        static constexpr auto SIGNED_N = static_cast<difference_type>(N);

        // ── Methods ─────────────────────────────────────────────────────────
        [[gnu::always_inline]] [[nodiscard]]
        static constexpr auto _move_ptr(
            const_pointer ptr,
            const difference_type offset
        ) noexcept -> pointer {
            if constexpr (N == 0) {
                return ptr;
            }

            return const_cast<pointer>(
                ptr + (((offset % SIGNED_N) + SIGNED_N) % SIGNED_N)
            );
        }

        template<std::random_access_iterator Iterator = iterator,
                 std::predicate<value_type, value_type> Predicate>
        constexpr void _sort(Iterator first, Iterator last, Predicate pred) {
            std::sort(first, last, pred);
        }

        template<std::random_access_iterator Iterator = iterator,
                 std::predicate<value_type, value_type> Predicate>
        constexpr void _stable_sort(
            Iterator first,
            Iterator last,
            Predicate pred
        ) {
            std::stable_sort(first, last, pred);
        }

    public:
        // ── iterator ────────────────────────────────────────────────────────
        class iterator {
        public:
            // ── Aliases ─────────────────────────────────────────────────────
            using iterator_category = std::random_access_iterator_tag;

            using iterator_concept  = std::random_access_iterator_tag;

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

            difference_type offset_;

            // ── Constructors ────────────────────────────────────────────────
            constexpr iterator(
                const_pointer   start,
                pointer pos,
                difference_type offset
            ) noexcept : start_(start), pos_(pos), offset_(offset) {}

        public:
            // ── Constructors ────────────────────────────────────────────────
            constexpr iterator()
                noexcept : start_(nullptr), pos_(nullptr), offset_(0) {}

            explicit constexpr iterator(
                circular_array& arr
            ) noexcept : start_(arr.values_), pos_(arr.values_), offset_(0) {}

            constexpr iterator(
                circular_array& arr,
                difference_type offset
            ) noexcept : start_(arr.values_),
                         pos_(_move_ptr(arr.values_, offset)),
                         offset_(offset) {}

            constexpr iterator(const iterator&) noexcept = default;

            constexpr iterator(iterator&&) noexcept = default;

            // ── Overloaded Operators ────────────────────────────────────────
            constexpr auto operator=(
                const iterator&
            ) noexcept -> iterator& = default;

            constexpr auto operator=(
                iterator&&
            ) noexcept -> iterator& = default;

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator==(
                const iterator& rhs
            ) const noexcept -> bool { return this->offset_ == rhs.offset_; }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator<=>(const iterator& rhs) const noexcept {
                return this->offset_ <=> rhs.offset_;
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator*() const noexcept -> reference {
                return *this->pos_;
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator->() const noexcept -> pointer {
                return this->pos_;
            }

            constexpr auto operator++() noexcept -> iterator& {
                ++this->offset_;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            constexpr auto operator++(int) noexcept -> iterator {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr auto operator--() noexcept -> iterator& {
                --this->offset_;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            constexpr auto operator--(int) noexcept -> iterator {
                iterator tmp = *this;
                --*this;
                return tmp;
            }

            constexpr auto operator+=(
                difference_type n
            ) noexcept -> iterator& {
                this->offset_ += n;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            [[nodiscard]]
            constexpr auto operator+(
                const difference_type n
            ) const noexcept -> iterator {
                const difference_type off = this->offset_ + n;
                return iterator{this->start_, _move_ptr(this->start_, off), off};
            }

            friend constexpr auto operator+(
                const difference_type n,
                const iterator& it
            ) noexcept -> iterator { return it + n; }

            constexpr auto operator-=(
                const difference_type n
            ) noexcept -> iterator& {
                this->offset_ -= n;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            [[nodiscard]]
            constexpr auto operator-(
                const difference_type n
            ) const noexcept -> iterator {
                const difference_type offset = this->offset_ - n;
                return iterator{
                    this->start_,
                    _move_ptr(this->start_, offset),
                    offset
                };
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator-(
                const iterator& rhs
            ) const noexcept -> difference_type {
                return this->offset_ - rhs.offset_;
            }

            [[nodiscard]]
            constexpr auto operator[](
                const difference_type n
            ) const noexcept -> reference {
                return *_move_ptr(this->start_, this->offset_ + n);
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr operator pointer() const noexcept { return this->pos_; }
        };

        // ── const_iterator ──────────────────────────────────────────────────
        class const_iterator {
        public:
            // ── Aliases ─────────────────────────────────────────────────────
            using iterator_category = std::random_access_iterator_tag;

            using iterator_concept  = std::random_access_iterator_tag;

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

            difference_type offset_;

            // ── Constructors ────────────────────────────────────────────────
            constexpr const_iterator(
                const_pointer start,
                const_pointer pos,
                difference_type offset
            ) noexcept : start_(start), pos_(pos), offset_(offset) {}

        public:
            // ── Constructors ────────────────────────────────────────────────
            constexpr const_iterator() noexcept
                : start_(nullptr), pos_(nullptr), offset_(0) {}

            constexpr const_iterator(const iterator& it) noexcept
                : start_(it.start_), pos_(it.pos_), offset_(it.offset_) {}

            constexpr const_iterator(const const_iterator&) noexcept = default;

            constexpr const_iterator(const_iterator&&) noexcept = default;

            // ── Overloaded Operators ────────────────────────────────────────
            constexpr auto operator=(
                const const_iterator&
            ) noexcept -> const_iterator& = default;
            
            constexpr auto operator=(
                const_iterator&&
            ) noexcept -> const_iterator& = default;

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator==(
                const const_iterator& rhs
            ) const noexcept -> bool { return this->offset_ == rhs.offset_; }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator<=>(
                const const_iterator& rhs
            ) const noexcept { return this->offset_ <=> rhs.offset_; }


            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator==(
                const iterator& rhs
            ) const noexcept -> bool { return this->offset_ == rhs.offset_; }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator<=>(const iterator& rhs) const noexcept {
                return this->offset_ <=> rhs.offset_;
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator*() const noexcept -> reference {
                return *this->pos_;
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator->() const noexcept -> pointer {
                return this->pos_;
            }

            constexpr auto operator++() noexcept -> const_iterator& {
                ++this->offset_;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            constexpr auto operator++(int) noexcept -> const_iterator {
                const_iterator tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr auto operator--() noexcept -> const_iterator& {
                --this->offset_;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            constexpr auto operator--(int) noexcept -> const_iterator {
                const_iterator tmp = *this;
                --*this;
                return tmp;
            }

            constexpr auto operator+=(
                const difference_type n
            ) noexcept -> const_iterator& {
                this->offset_ += n;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            [[nodiscard]]
            constexpr auto operator+(
                const difference_type n
            ) const noexcept -> const_iterator {
                const difference_type offset = this->offset_ + n;
                return const_iterator{
                    this->start_,
                    _move_ptr(this->start_, offset), offset
                };
            }

            friend constexpr auto operator+(
                const difference_type n,
                const const_iterator& it
            ) noexcept -> const_iterator { return it + n; }

            constexpr auto operator-=(
                const difference_type n
            ) noexcept -> const_iterator& {
                this->offset_ -= n;
                this->pos_ = _move_ptr(this->start_, this->offset_);
                return *this;
            }

            [[nodiscard]]
            constexpr auto operator-(
                const difference_type n
            ) const noexcept -> const_iterator {
                const difference_type offset = this->offset_ - n;
                return const_iterator{
                    this->start_,
                    _move_ptr(this->start_, offset),
                    offset
                };
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator-(
                const const_iterator& rhs
            ) const noexcept -> difference_type {
                return this->offset_ - rhs.offset_;
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr auto operator-(
                const iterator& rhs
            ) const noexcept -> difference_type {
                return this->offset_ - rhs.offset_;
            }

            [[gnu::always_inline]] [[nodiscard]]
            friend constexpr auto operator-(
                const iterator& lhs,
                const const_iterator& rhs
            ) noexcept -> difference_type {
                return lhs.offset_ - rhs.offset_;
            }

            [[nodiscard]]
            constexpr auto operator[](
                const difference_type n
            ) const noexcept -> reference {
                return *_move_ptr(this->start_, this->offset_ + n);
            }

            [[gnu::always_inline]] [[nodiscard]]
            constexpr operator const_pointer() const noexcept {
                return this->pos_;
            }
        };

        // ── Overloaded Operators ────────────────────────────────────────────
        [[nodiscard]]
        constexpr auto operator==(const circular_array& rhs) const noexcept(
            noexcept(std::declval<value_type>() == std::declval<value_type>())
        ) -> bool {
            return std::equal(this->begin(), this->end(), rhs.begin());
        }

        [[nodiscard]]
        constexpr auto operator<=>(
            const circular_array& rhs
        ) const noexcept(
            noexcept(std::declval<value_type>() <=> std::declval<value_type>())
        ) -> std::compare_three_way_result_t<value_type> {
            return std::lexicographical_compare_three_way(
                this->values_, this->values_ + N,
                rhs.values_, rhs.values_ + N
            );
        }
        
        [[nodiscard]]
        constexpr auto operator[](
            difference_type index
        ) noexcept -> reference {
            return *_move_ptr(this->values_, index);
        }

        [[nodiscard]]
        constexpr auto operator[](
            difference_type index
        ) const noexcept -> const_reference {
            return *_move_ptr(this->values_, index);
        }

        // ── Methods ─────────────────────────────────────────────────────────
        [[nodiscard]]
        constexpr auto at(difference_type index) noexcept -> reference {
            return *_move_ptr(this->values_, index);
        }

        [[nodiscard]]
        constexpr auto at(
            difference_type index
        ) const noexcept -> const_reference {
            return *_move_ptr(this->values_, index);
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto front() noexcept -> reference {
            return this->values_[0];
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto front() const noexcept -> const_reference {
            return this->values_[0];
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto back() noexcept -> reference {
            return this->values_[N - 1];
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto back() const noexcept -> const_reference {
            return this->values_[N - 1];
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto data() noexcept -> pointer {
            return this->values_;
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto data() const noexcept -> const_pointer {
            return this->values_;
        }

        [[nodiscard]]
        constexpr auto begin() noexcept -> iterator {
            return iterator{this->values_, this->values_, 0};
        }

        [[nodiscard]]
        constexpr auto end() noexcept -> iterator {
            return iterator{this->values_, this->values_ + N, SIGNED_N};
        }

        [[nodiscard]]
        constexpr auto begin() const noexcept -> const_iterator {
            return const_iterator{this->values_, this->values_, 0};
        }

        [[nodiscard]]
        constexpr auto end() const noexcept -> const_iterator {
            return const_iterator{this->values_, this->values_ + N, SIGNED_N};
        }

        [[nodiscard]]
        constexpr auto cbegin() const noexcept -> const_iterator {
            return this->begin();
        }

        [[nodiscard]]
        constexpr auto cend() const noexcept -> const_iterator {
            return this->end();
        }

        [[nodiscard]]
        constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(this->end());
        }

        [[nodiscard]]
        constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(this->begin());
        }

        [[nodiscard]]
        constexpr auto rbegin() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->end());
        }

        [[nodiscard]]
        constexpr auto rend() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->begin());
        }
        [[nodiscard]]
        constexpr auto crbegin() const noexcept -> const_reverse_iterator {
            return this->rbegin();
        }
        
        [[nodiscard]]
        constexpr auto crend() const noexcept -> const_reverse_iterator {
            return this->rend();
        }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto empty() const noexcept -> bool { return N == 0; }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto size() const noexcept -> size_type { return N; }

        [[gnu::always_inline]] [[nodiscard]]
        constexpr auto max_size() const noexcept -> size_type { return N; }

        constexpr void fill(
            const_reference value
        ) noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            std::fill(begin(), end(), value);
        }

        constexpr void swap(circular_array& other)
            noexcept(std::is_nothrow_swappable_v<value_type>) {
            std::swap(this->values_, other.values_);
        }

    
        constexpr void sort() {
            this->_sort(begin(), end(), std::less<value_type>{});
        }

        template<std::random_access_iterator Iterator = iterator>
        constexpr void sort(Iterator first, Iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_sort(first, last, std::less<value_type>{});
        }

        template<std::random_access_iterator Iterator = iterator,
                 std::predicate<value_type, value_type> Predicate>
        constexpr void sort(Iterator first, Iterator last, Predicate pred) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_sort(first, last, pred);
        }

        constexpr void stable_sort() {
            this->_stable_sort(begin(), end(), std::less<value_type>{});
        }

         template<std::random_access_iterator Iterator = iterator>
        constexpr void stable_sort(Iterator first, Iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }

            this->_stable_sort(first, last, std::less<value_type>{});
        }

         template<std::random_access_iterator Iterator = iterator,
                 std::predicate<value_type, value_type> Predicate>
        constexpr void stable_sort(
            Iterator first,
            Iterator last,
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