#pragma once

#include <cstddef>
#include <stdexcept>

#include <iterator.hpp>

namespace tp {

template <typename T, std::size_t N> struct array {
	using value_type      = T;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference       = value_type&;
	using const_reference = const value_type;
	using pointer         = value_type*;
	using const_pointer   = const value_type*;

	using const_iterator = tp::normal_iterator<const_pointer, array<T,N>>;
	using iterator = tp::normal_iterator<pointer, array<T, N>>;
	using const_reverse_iterator = tp::reverse_iterator<const_iterator>;
	using reverse_iterator = tp::reverse_iterator<iterator>;

	reference at(size_type pos) {
		if (pos >= N)
			throw std::out_of_range("out of range\n");
		return elems[pos];
	}

	const_reference at(size_type pos) const {
		if (pos >= N)
			throw std::out_of_range("out of range\n");
		return elems[pos];
	}

	reference operator[](size_type pos) {
		return elems[pos];
	}

	const_reference operator[](size_type pos) const {
		return elems[pos];
	}

	reference front() {
		return elems[0];
	}

	const_reference front() const {
		return elems[0];
	}

	reference back() {
		return elems[N - 1];
	}

	const_reference back() const {
		return elems[N - 1];
	}

	T* data() {
		return elems;
	}

	const T* data() const {
		return elems;
	}

	iterator begin() {
		return iterator(elems);
	}

	const_iterator begin() const {
		return const_iterator(elems);
	}

	const_iterator cbegin() {
		return const_iterator(elems);
	}

	iterator end() {
		return iterator(elems + N);
	}

	const_iterator end() const {
		return const_iterator(elems + N);
	}

	const_iterator cend() {
		return const_iterator(elems + N);
	}

	reverse_iterator rbegin() {
		return reverse_iterator(elems + N - 1);
	}

	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(elems + N - 1);
	}

	const_reverse_iterator crbegin() {
		return const_reverse_iterator(elems + N - 1);
	}

	reverse_iterator rend() {
		return reverse_iterator(elems - 1);
	}

	const_reverse_iterator rend() const {
		return const_reverse_iterator(elems - 1);
	}

	const_reverse_iterator crend() {
		return const_reverse_iterator(elems - 1);
	}


	bool empty() const {
		return N == 0;
	}

	size_type size() const {
		return N;
	}

	void fill(const T& value) {
		for (size_type i = 0; i < N; ++i)
			elems[i] = value;
	}

	T elems[N];
};
} // namespace tp
