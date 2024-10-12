#pragma once

#include <iterator>

namespace tp{

template <typename T>
concept is_iterator = requires(T a, T b) {
	*a;
	++a;
};

template <typename Iterator, typename Container> class normal_iterator {
	using traits_type = std::iterator_traits<Iterator>;

public:
	using difference_type   = traits_type::difference_type;
	using value_type        = traits_type::value_type;
	using pointer           = traits_type::pointer;
	using reference         = traits_type::reference;
	using iterator_category = traits_type::iterator_category;

	normal_iterator() : cur(Iterator()) {}
	normal_iterator(const Iterator &it) : cur(it) {}

	template <typename Iter>
		requires std::convertible_to<Iter, Iterator>
	normal_iterator(const normal_iterator<Iter, Container> &it) : cur(it.base()) {}

	reference operator*() { return *cur; }

	pointer operator->() { return cur; }

	normal_iterator &operator++() {
		++cur;
		return *this;
	}

	normal_iterator operator++(int) { return normal_iterator(cur++); }

	normal_iterator &operator--() {
		--cur;
		return *this;
	}

	normal_iterator operator--(int) { return normal_iterator(cur--); }

	// random access iterator required;
	reference operator[](difference_type n) { return cur[n]; }

	normal_iterator &operator+=(difference_type n) {
		cur += n;
		return *this;
	}

	normal_iterator operator+(difference_type n) {
		return normal_iterator(cur + n);
	}

	normal_iterator &operator-=(difference_type n) {
		cur -= n;
		return *this;
	}

	normal_iterator operator-(difference_type n) {
		return normal_iterator(cur - n);
	}

	const Iterator &base() const { return cur; }

private:
	Iterator cur;
};

// below are six comparison funcs
template <typename IteratorL, typename IteratorR, typename Container>
bool operator==(const normal_iterator<IteratorL, Container> &lhs,
                const normal_iterator<IteratorR, Container> &rhs) {
	return lhs.base() == rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
bool operator!=(const normal_iterator<IteratorL, Container> &lhs,
                const normal_iterator<IteratorR, Container> &rhs) {
	return lhs.base() != rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
bool operator>(const normal_iterator<IteratorL, Container> &lhs,
               const normal_iterator<IteratorR, Container> &rhs) {
	return (lhs.base() > rhs.base());
}

template <typename IteratorL, typename IteratorR, typename Container>
bool operator<(const normal_iterator<IteratorL, Container> &lhs,
               const normal_iterator<IteratorR, Container> &rhs) {
	return lhs.base() < rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
bool operator<=(const normal_iterator<IteratorL, Container> &lhs,
                const normal_iterator<IteratorR, Container> &rhs) {
	return lhs.base() <= rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
bool operator>=(const normal_iterator<IteratorL, Container> &lhs,
                const normal_iterator<IteratorR, Container> &rhs) {
	return lhs.base() >= rhs.base();
}

// + and -

template <typename IteratorL, typename IteratorR, typename Container>
auto operator-(const normal_iterator<IteratorL, Container> &lhs,
               const normal_iterator<IteratorR, Container> &rhs)
    -> decltype(lhs.base() - rhs.base()) {
	return lhs.base() - rhs.base();
}

template <typename Iterator, typename Container>
normal_iterator<Iterator, Container>
operator+(typename normal_iterator<Iterator, Container>::difference_type &n,
          const normal_iterator<Iterator, Container> &it) {
	return normal_iterator(it.base() + n);
}

template <typename Iterator, typename Container>
normal_iterator<Iterator, Container>
operator+(const normal_iterator<Iterator, Container> &it,
          typename normal_iterator<Iterator, Container>::difference_type &n) {
	return normal_iterator(it.base() + n);
}

// reverse_iterator
template <typename Iterator> class reverse_iterator {
	using traits_type = std::iterator_traits<Iterator>;

public:
	using difference_type   = traits_type::difference_type;
	using value_type        = traits_type::value_type;
	using pointer           = traits_type::pointer;
	using reference         = traits_type::reference;
	using iterator_category = traits_type::iterator_category;

	reverse_iterator() : cur(Iterator()) {}
	reverse_iterator(const Iterator &it) : cur(it) {}
	reverse_iterator(const reverse_iterator &it) : cur(it.cur) {}

	reference operator*() { return *cur; }

	pointer operator->() { return cur; }

	reverse_iterator &operator++() {
		--cur;
		return *this;
	}

	reverse_iterator operator++(int) { return reverse_iterator(cur--); }

	reverse_iterator &operator--() {
		++cur;
		return *this;
	}

	reverse_iterator operator--(int) { return reverse_iterator(cur++); }

	// random access iterator required;
	reference operator[](difference_type n) { return *(*this + n); }

	reverse_iterator &operator+=(difference_type n) {
		cur -= n;
		return *this;
	}

	reverse_iterator operator+(difference_type n) {
		return reverse_iterator(cur - n);
	}

	reverse_iterator &operator-=(difference_type n) {
		cur += n;
		return *this;
	}

	reverse_iterator operator-(difference_type n) {
		return reverse_iterator(cur + n);
	}

	const Iterator &base() const { return cur; }

private:
	Iterator cur;
};

// below are six comparison funcs
template <typename IteratorL, typename IteratorR>
bool operator==(const reverse_iterator<IteratorL> &lhs,
                const reverse_iterator<IteratorR> &rhs) {
	return lhs.base() == rhs.base();
}

template <typename IteratorL, typename IteratorR>
bool operator!=(const reverse_iterator<IteratorL> &lhs,
                const reverse_iterator<IteratorR> &rhs) {
	return lhs.base() != rhs.base();
}

template <typename IteratorL, typename IteratorR>
bool operator>(const reverse_iterator<IteratorL> &lhs,
               const reverse_iterator<IteratorR> &rhs) {
	return lhs.base() < rhs.base();
}

template <typename IteratorL, typename IteratorR>
bool operator<(const reverse_iterator<IteratorL> &lhs,
               const reverse_iterator<IteratorR> &rhs) {
	return lhs.base() > rhs.base();
}

template <typename IteratorL, typename IteratorR>
bool operator<=(const reverse_iterator<IteratorL> &lhs,
                const reverse_iterator<IteratorR> &rhs) {
	return lhs.base() >= rhs.base();
}

template <typename IteratorL, typename IteratorR>
bool operator>=(const reverse_iterator<IteratorL> &lhs,
                const reverse_iterator<IteratorR> &rhs) {
	return lhs.base() <= rhs.base();
}

// + and -

template <typename IteratorL, typename IteratorR>
auto operator-(const reverse_iterator<IteratorL> &lhs,
               const reverse_iterator<IteratorR> &rhs)
    -> decltype(lhs.base() - rhs.base()) {
	return -(lhs.base() - rhs.base());
}

template <typename Iterator>
reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type &n,
          const reverse_iterator<Iterator> &it) {
	return reverse_iterator(it.base() - n);
}

template <typename Iterator>
reverse_iterator<Iterator>
operator+(const reverse_iterator<Iterator> &it,
          typename reverse_iterator<Iterator>::difference_type &n) {
	return reverse_iterator(it.base() - n);
}
} // namespace tp
