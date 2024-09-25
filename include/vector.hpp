#pragma once

#include <memory>

template <typename T, typename Alloc = std::allocator<T>> class vector {
public:
	using value_type      = T;
	using allocator_type  = Alloc;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference       = T &;
	using const_reference = const T &;
	using pointer         = std::allocator_traits<Alloc>::pointer;
	using const_pointer   = std::allocator_traits<Alloc>::const_pointer;
	// using iterator =
	// using const_iterator =
	// using reverse_iterator =
	// using const_reverse_iterator =

	vector() : alloc{}, sz(0), cap(0), data(nullptr) {}

	vector(size_type count, const T &value, const Alloc &alloc = Alloc())
	    : alloc(alloc), sz(count), cap(count) {
		data = std::allocator_traits<Alloc>::allocate(alloc, count);
		for (size_type i = 0; i < count; ++i)
			std::allocator_traits<Alloc>::construct(alloc, data + i, value);
	}

	explicit vector(size_type count, const Alloc &alloc = Alloc())
	    : alloc(alloc), sz(count), cap(count) {
			data = std::allocator_traits<Alloc>::allocate(alloc, count);
		}

	vector(std::initializer_list<T> init, const Alloc& alloc = Alloc()): alloc(alloc) {
		size_type size = init.size();
		sz = cap = size;
		data = std::allocator_traits<Alloc>::allocate(alloc, size);
		for(size_type i = 0; i < size; ++i) {
			std::allocator_traits<Alloc>::construct(alloc, data + i, init[i]);
		}
	}

	// copy ctor
	vector(const vector& other) {
	}

	vector(const vector& other, const Alloc& alloc) {

	}

	// move ctor
	vector(vector&& other) {
	}

	// dtor
	~vector() {
	}

	// copy assign
	vector& operator=(const vector& other) {
	}

	// move assing
	vector& operator=(vector&& other) {
	}

	vector& operator=(std::initializer_list<value_type> ilist) {
	}

	reference at(size_type pos) {
	}

	const_reference at(size_type pos) const {
	}

	value_type operator[](size_type pos) {
	}

	bool empty() const {return sz == 0;}

	size_type size() const { return sz; }

	void reserve(size_type new_cap) {
		if (new_cap > cap) {
		}
		else {
		}
	}

	size_type capacity() const {return cap; }

	void shrink_to_fit() {
	}

private:
	void reallocate() {
		size_type new_cap = 2 * cap;

		T* new_data = std::allocator_traits<Alloc>::allocate(alloc, new_cap);
		for (size_type i = 0; i < sz; ++i) {
			std::allocator_traits<Alloc>::construct(alloc, new_data + i, std::move(data[i]));
		}
		std::allocator_traits<Alloc>::deallocate(alloc, data, cap);
	}

	Alloc alloc;
	size_type sz;
	size_type cap;
	T *data;
};
