#pragma once

#include <memory>

#include <iterator.hpp>


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

	/* class iterator;
	 * class const_iterator;
	 * class reverse_iterator;
	 * class const_reverse_iterator; */
	using iterator = tp::normal_iterator<pointer, vector>;
	using const_iterator = tp::normal_iterator<const_pointer, vector>;
	using reverse_iterator = tp::reverse_iterator<iterator>;
	using const_reverse_iterator = tp::reverse_iterator<const_iterator>;

	vector() : alloc{}, sz(0), cap(0), _data(nullptr) {}

	vector(size_type count, const T &value, const Alloc &_alloc = Alloc())
	    : alloc(_alloc), sz(count), cap(count) {
		_data = std::allocator_traits<Alloc>::allocate(alloc, count);
		for (size_type i = 0; i < count; ++i)
			std::allocator_traits<Alloc>::construct(alloc, _data + i, value);
	}

	explicit vector(size_type count, const Alloc &_alloc = Alloc())
	    : alloc(_alloc), sz(count), cap(count) {
		_data = std::allocator_traits<Alloc>::allocate(alloc, count);
	}

	vector(std::initializer_list<T> init, const Alloc &_alloc = Alloc())
	    : alloc(_alloc) {
		size_type size = init.size();
		sz = cap = size;
		_data    = std::allocator_traits<Alloc>::allocate(alloc, size);

		auto it = init.begin();
		T* dst = _data;
		while (it != init.end()) {
			std::allocator_traits<Alloc>::construct(alloc, dst, *it);
			++dst, ++it;
		}
	}

	// copy ctor
	vector(const vector &other)
	    : alloc(std::allocator_traits<
	            Alloc>::select_on_container_copy_construction(other.alloc)),
	      sz(other.sz), cap(other.cap) {
		_data = std::allocator_traits<Alloc>::allocate(alloc, cap);
		for (size_type i = 0; i < sz; ++i) {
			std::allocator_traits<Alloc>::construct(alloc, _data + i, other[i]);
		}
	}

	vector(const vector &other, const Alloc &_alloc)
	    : alloc(_alloc), sz(other.sz), cap(other.cap) {
		_data = std::allocator_traits<Alloc>::allocate(alloc, cap);
		for (size_type i = 0; i < sz; ++i) {
			std::allocator_traits<Alloc>::construct(alloc, _data + i, other[i]);
		}
	}

	/*
	 * Move constructor.
	 * Constructs the container with the contents of other using move semantics.
	 * Allocator is obtained by move-construction from the allocator belonging
	 * to other. After the move, other is guaranteed to be empty().
	 */
	vector(vector &&other)
	    : alloc(std::move(other.alloc)), sz(other.sz), cap(other.cap),
	      _data(other._data) {
		other.data = nullptr;
		other.sz   = 0;
		other.cap  = 0;
	}

	// dtor
	~vector() {
		for (size_type i = 0; i < sz; ++i)
			std::allocator_traits<Alloc>::destroy(alloc, _data + i);
		std::allocator_traits<Alloc>::deallocate(alloc, _data, cap);
	}

	// copy assign
	/**
	  If
	  std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value
	  is true, the allocator of *this is replaced by a copy of other. If the
	  allocator of *this after assignment would compare unequal to its old
	  value, the old allocator is used to deallocate the memory, then the new
	  allocator is used to allocate it before copying the elements. Otherwise,
	  the memory owned by *this may be reused when possible. In any case, the
	  elements originally belonging to *this may be either destroyed or replaced
	  by element-wise copy-assignment.
	*/
	vector &operator=(const vector &other) {
		if (std::allocator_traits<
		        Alloc>::propagate_on_container_copy_assignment::value == true) {
			Alloc old_alloc = alloc;
			alloc           = other.alloc;
			if (alloc != old_alloc) {
				// use old_alloc release memory
				for (size_type i = 0; i < sz; ++i)
					std::allocator_traits<Alloc>::destroy(old_alloc, _data + i);
				std::allocator_traits<Alloc>::deallocate(old_alloc, _data, cap);

				// use new alloc allocate memory
				sz = cap = other.size();
				_data    = std::allocator_traits<Alloc>::allocate(alloc, cap);
				for (size_type i = 0; i < sz; ++i)
					std::allocator_traits<Alloc>::construct(alloc, _data + i,
					                                        other[i]);
			}
		} else {
			if (other.size() > cap) {
				reallocate(other.size());
				cap = other.size();
			}
			sz = other.size();
			for (size_type i = 0; i < sz; ++i)
				_data[i] = other[i];
		}
		return *this;
	}

	// move assign
	/*
	  Move assignment operator.
	  Replaces the contents with those of other using move semantics (i.e. the
	  data in other is moved from other into this container). other is in a
	  valid but unspecified state afterwards.

	  If
	  std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value
	  is true, the allocator of *this is replaced by a copy of that of other. If
	  it is false and the allocators of *this and other do not compare equal,
	    *this cannot take ownership of the memory owned by other and must
	  move-assign each element individually, allocating additional memory using
	  its own allocator as needed. In any case, all elements originally
	  belonging to *this are either destroyed or replaced by element-wise
	  move-assignment.
	*/
	vector &operator=(vector &&other) {
		if (std::allocator_traits<
		        Alloc>::propagate_on_container_move_assignment::value ==
		        false &&
		    alloc != other.alloc) {
			for (size_type i = 0; i < sz; ++i)
				std::allocator_traits<Alloc>::destroy(alloc, _data + i);
			if (other.sz > cap)
				_data = reallocate(other.sz);
			sz = other.sz;
			for (size_type i = 0; i < sz; ++i)
				std::allocator_traits<Alloc>::construct(
				    alloc, _data + i, std::move(*(other._data + i)));
			std::allocator_traits<Alloc>::deallocate(other.alloc, other._data,
			                                         other.cap);

			other.sz    = 0;
			other.cap   = 0;
			other._data = nullptr;
		} else {
			if (_data) {
				for (size_type i = 0; i < sz; ++i)
					std::allocator_traits<Alloc>::destroy(alloc, _data + i);
				std::allocator_traits<Alloc>::deallocate(alloc, _data, cap);
			}
			alloc = other.alloc;
			sz    = other.sz;
			cap   = other.cap;
			_data = other._data;

			other.sz    = 0;
			other.cap   = 0;
			other._data = nullptr;
		}
		return *this;
	}

	vector &operator=(std::initializer_list<value_type> ilist) {
		size_type size = ilist.size();
		if (size > cap)
			_data = reallocate(size);
		sz = size;

		auto it = ilist.begin();
		T *dst  = _data;
		while (it != ilist.end()) {
			std::allocator_traits<Alloc>::construct(alloc, dst, *it);
			++dst, ++it;
		}
		return *this;
	}

	reference at(size_type pos) {
		if (pos >= sz)
			throw std::out_of_range("out of range\n");
		return _data[pos];
	}

	const_reference at(size_type pos) const {
		if (pos >= sz)
			throw std::out_of_range("out of range\n");
		return _data[pos];
	}

	reference operator[](size_type pos) { return _data[pos]; }

	const_reference operator[](size_type pos) const { return _data[pos]; }

	reference front() { return *_data; }

	const_reference front() const { return *_data; }

	reference back() { return *(_data + sz - 1); }

	const_reference back() const { return *(_data + sz - 1); }

	T *data() { return _data; }

	const T *data() const { return _data; }

	iterator begin() { return iterator(_data); }

	iterator begin() const { return iterator(_data); }

	const_iterator cbegin() { return const_iterator(_data); }

	iterator end() { return iterator(_data + sz); }

	const_iterator cend() { return const_iterator(_data + sz); }

	reverse_iterator rbegin() { return reverse_iterator(_data + sz - 1); }

	reverse_iterator rbegin() const { return reverse_iterator(_data + sz - 1); }

	const_reverse_iterator crbegin() {
		return const_reverse_iterator(_data + sz - 1);
	}

	reverse_iterator rend() { return reverse_iterator(_data - 1); }

	const_reverse_iterator crend() { return const_reverse_iterator(_data - 1); }

	bool empty() const { return sz == 0; }

	size_type size() const { return sz; }

	void reserve(size_type new_cap) {
		if (new_cap > cap) {
			_data = reallocate(new_cap);
			cap   = new_cap;
		}
	}

	size_type capacity() const { return cap; }

	void shrink_to_fit() {
		_data = reallocate(sz);
		cap   = sz;
	}

	void clear() { sz = 0; }

	iterator insert(const_iterator pos, const T &value);

	iterator insert(const_iterator pos, T &&value);

	iterator insert(const_iterator pos, size_type count, const T &value);

	template <typename InputIt>
	requires is_iterator<InputIt> iterator insert(const_iterator pos,
	                                              InputIt first, InputIt last);

	iterator insert(const_iterator pos, std::initializer_list<T> ilist);

	iterator erase(const_iterator pos);

	iterator erase(const_iterator first, const_iterator last);

	void push_back(const T &value) {
		if (sz == cap)
			_data = reallocate();
		std::allocator_traits<Alloc>::construct(alloc, _data + sz, value);
		++sz;
	}

	void push_back(T &&value) {
		if (sz == cap)
			_data = reallocate();
		std::allocator_traits<Alloc>::construct(alloc, _data + sz,
		                                        std::move(value));
		++sz;
	}

	template <typename... Args> reference emplace_back(Args &&...args) {
		if (sz == cap)
			_data = reallocate();
		std::allocator_traits<Alloc>::construct(alloc, _data + sz,
		                                        std::forward<Args>(args)...);
		++sz;
		return *(_data + sz - 1);
	}

	void pop_back() { --sz; }

	void resize(size_type count) {
		if (sz > count) {
			sz = count;
		} else if (sz < count) {
			if (cap < count)
				_data = reallocate(count);
			for (size_type i = 0; i < count; ++i)
				// call default ctor
				std::allocator_traits<Alloc>::construct(alloc, _data + i);
		}
	}

	void resize(size_type count, const value_type &value) {
		if (sz > count)
			sz = count;
		else if (sz < count) {
			if (cap < count)
				_data = reallocate(count);
			for (size_type i = 0; i < count; ++i)
				// call copy ctor
				std::allocator_traits<Alloc>::construct(alloc, _data + i,
				                                        value);
		}
	}

	void swap(vector &other) {
		std::swap(_data, other._data);
		std::swap(sz, other.sz);
		std::swap(cap, other.cap);

		if (std::allocator_traits<Alloc>::propagate_on_container_swap::value) {
			// by ADL to find swap
			swap(alloc, other.alloc);
		}
	}

private:
	T *reallocate() {
		size_type new_cap = 2 * cap;
		return reallocate(new_cap);
	}

	T *reallocate(size_type new_cap) {
		T *new_data = std::allocator_traits<Alloc>::allocate(alloc, new_cap);
		for (size_type i = 0; i < sz; ++i) {
			std::allocator_traits<Alloc>::construct(alloc, new_data + i,
			                                        std::move(_data[i]));
		}
		std::allocator_traits<Alloc>::deallocate(alloc, _data, cap);

		cap = new_cap;
		return new_data;
	}

	Alloc alloc;
	size_type sz;
	size_type cap;
	T *_data;
};

/* template <typename T, typename Alloc> class vector<T, Alloc>::iterator {
 * public:
 *     iterator() : ptr(nullptr) {}
 * 
 *     iterator(T *p) : ptr(p) {}
 * 
 *     iterator(const iterator &other) : ptr(other.ptr) {}
 *     iterator(iterator &&other) : ptr(other.ptr) {}
 * 
 *     iterator &operator=(const iterator &other) {
 *         ptr = other.ptr;
 *         return *this;
 *     }
 * 
 *     iterator &operator=(iterator &&other) {
 *         ptr = other.ptr;
 *         return *this;
 *     }
 * 
 *     auto operator<=>(const iterator &other) const = default;
 * 
 *     iterator &operator++() {
 *         ptr += 1;
 *         return *this;
 *     }
 * 
 *     iterator operator++(int) {
 *         iterator ret = *this;
 *         ptr += 1;
 *         return ret;
 *     }
 * 
 *     iterator &operator--() {
 *         ptr -= 1;
 *         return *this;
 *     }
 * 
 *     iterator operator--(int) {
 *         iterator ret = *this;
 *         ptr -= 1;
 *         return ret;
 *     }
 * 
 *     friend iterator operator+(iterator it, int cnt) {
 *         return iterator(it.ptr + cnt);
 *     }
 * 
 *     friend iterator operator+(int cnt, iterator it) {
 *         return iterator(it.ptr + cnt);
 *     }
 * 
 *     friend iterator operator-(iterator it, int cnt) {
 *         return iterator(it.ptr - cnt);
 *     }
 * 
 *     friend difference_type operator-(iterator &lhs, iterator &rhs) {
 *         return lhs.ptr - rhs.ptr;
 *     }
 * 
 *     reference operator*() { return *ptr; }
 * 
 *     pointer operator->() { return ptr; }
 * 
 * private:
 *     T *ptr;
 * };
 * 
 * template <typename T, typename Alloc> class vector<T, Alloc>::const_iterator {
 * public:
 *     const_iterator() : ptr(nullptr) {}
 * 
 *     const_iterator(T *p) : ptr(p) {}
 *     const_iterator(const T *p) : ptr(p) {}
 * 
 *     const_iterator &operator=(const const_iterator &other) {
 *         ptr = other.ptr;
 *         return *this;
 *     }
 * 
 *     const_iterator &operator=(const_iterator &&other) {
 *         ptr = other.ptr;
 *         return *this;
 *     }
 * 
 *     auto operator<=>(const const_iterator &other) const = default;
 * 
 *     const_iterator &operator++() {
 *         ptr += 1;
 *         return *this;
 *     }
 * 
 *     const_iterator operator++(int) {
 *         const_iterator ret = *this;
 *         ptr += 1;
 *         return ret;
 *     }
 * 
 *     const_iterator &operator--() {
 *         ptr -= 1;
 *         return *this;
 *     }
 * 
 *     const_iterator operator--(int) {
 *         const_iterator ret = *this;
 *         ptr -= 1;
 *         return ret;
 *     }
 * 
 *     friend const_iterator operator+(const_iterator it, int cnt) {
 *         return const_iterator(it.ptr + cnt);
 *     }
 * 
 *     friend const_iterator operator+(int cnt, const_iterator it) {
 *         return const_iterator(it.ptr + cnt);
 *     }
 * 
 *     friend const_iterator operator-(const_iterator it, int cnt) {
 *         return const_iterator(it.ptr - cnt);
 *     }
 * 
 *     friend difference_type operator-(const const_iterator &lhs,
 *                                      const const_iterator &rhs) {
 *         return lhs.ptr - rhs.ptr;
 *     }
 * 
 *     const_reference operator*() { return *ptr; }
 * 
 *     const_pointer operator->() { return ptr; }
 * 
 * private:
 *     const T *ptr;
 * };
 * 
 * template <typename T, typename Alloc> class vector<T, Alloc>::reverse_iterator {
 * public:
 *     reverse_iterator() : it(nullptr) {}
 * 
 *     reverse_iterator(T *p) : it(p) {}
 * 
 *     reverse_iterator &operator=(const reverse_iterator &other) {
 *         it = other.it;
 *         return *this;
 *     }
 * 
 *     reverse_iterator &operator=(reverse_iterator &&other) {
 *         it = other.it;
 *         return *this;
 *     }
 * 
 *     auto operator<=>(const reverse_iterator &other) const = default;
 * 
 *     reverse_iterator &operator++() {
 *         --it;
 *         return *this;
 *     }
 * 
 *     reverse_iterator operator++(int) {
 *         reverse_iterator ret = *this;
 *         --(*this);
 *         return ret;
 *     }
 * 
 *     reverse_iterator &operator--() {
 *         ++it;
 *         return *this;
 *     }
 * 
 *     reverse_iterator operator--(int) {
 *         reverse_iterator ret = *this;
 *         ++(*this);
 *         return ret;
 *     }
 * 
 *     friend reverse_iterator operator+(reverse_iterator &rit, int cnt) {
 *         rit.it = rit.it - cnt;
 *         return rit;
 *     }
 * 
 *     friend reverse_iterator operator+(int cnt, reverse_iterator &rit) {
 *         rit.it = rit.it - cnt;
 *         return rit;
 *     }
 * 
 *     friend reverse_iterator operator-(reverse_iterator &rit, int cnt) {
 *         rit.it = rit.it + cnt;
 *         return rit;
 *     }
 * 
 *     friend difference_type operator-(reverse_iterator &lhs,
 *                                      reverse_iterator &rhs) {
 *         return -(lhs.it - rhs.it);
 *     }
 * 
 *     reference operator*() { return *it; }
 * 
 *     pointer operator->() { return it; }
 * 
 * private:
 *     iterator it;
 * };
 * 
 * template <typename T, typename Alloc>
 * class vector<T, Alloc>::const_reverse_iterator {
 * public:
 *     const_reverse_iterator() : it(nullptr) {}
 * 
 *     const_reverse_iterator(T *p) : it(p) {}
 * 
 *     const_reverse_iterator &operator=(const const_reverse_iterator &other) {
 *         it = other.it;
 *         return *this;
 *     }
 * 
 *     const_reverse_iterator &operator=(const_reverse_iterator &&other) {
 *         it = other.it;
 *         return *this;
 *     }
 * 
 *     auto operator<=>(const const_reverse_iterator &other) const = default;
 * 
 *     const_reverse_iterator &operator++() {
 *         --it;
 *         return *this;
 *     }
 * 
 *     const_reverse_iterator operator++(int) {
 *         const_reverse_iterator ret = *this;
 *         --(*this);
 *         return ret;
 *     }
 * 
 *     const_reverse_iterator &operator--() {
 *         ++it;
 *         return *this;
 *     }
 * 
 *     const_reverse_iterator operator--(int) {
 *         const_reverse_iterator ret = *this;
 *         ++(*this);
 *         return ret;
 *     }
 * 
 *     friend const_reverse_iterator operator+(const_reverse_iterator &rit,
 *                                             int cnt) {
 *         rit.it = rit.it - cnt;
 *         return rit;
 *     }
 * 
 *     friend const_reverse_iterator operator+(int cnt,
 *                                             const_reverse_iterator &rit) {
 *         rit.it = rit.it - cnt;
 *         return rit;
 *     }
 * 
 *     friend const_reverse_iterator operator-(const_reverse_iterator &rit,
 *                                             int cnt) {
 *         rit.it = rit.it + cnt;
 *         return rit;
 *     }
 * 
 *     friend difference_type operator-(const_reverse_iterator &lhs,
 *                                      const_reverse_iterator &rhs) {
 *         return -(lhs.it - rhs.it);
 *     }
 * 
 *     const_reference operator*() { return *it; }
 * 
 *     const_pointer operator->() { return it; }
 * 
 * private:
 *     const_iterator it;
 * }; */

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(typename vector<T, Alloc>::const_iterator pos,
                         const T &value) {

	size_type offset = pos - cbegin();

	if (sz == cap)
		_data = reallocate();

	T *flag = _data + offset;
	T *src = _data + sz - 1, *dst = _data + sz;

	while (src >= flag) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		--src, --dst;
	}

	std::allocator_traits<Alloc>::construct(alloc, flag, value);
	++sz;

	return iterator(flag);
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(typename vector<T, Alloc>::const_iterator pos,
                         T &&value) {
	size_type offset = pos - cbegin();

	if (sz == cap)
		_data = reallocate();

	T *flag = _data + offset;
	T *src = _data + sz - 1, *dst = _data + sz;

	while (src >= flag) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		--src, --dst;
	}

	std::allocator_traits<Alloc>::construct(alloc, flag, std::move(value));
	++sz;

	return begin() + offset;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(typename vector<T, Alloc>::const_iterator pos,
                         typename vector<T, Alloc>::size_type count,
                         const T &value) {
	size_type offset = pos - cbegin();

	if (sz + count > cap)
		_data = reallocate(sz + count);

	T *flag = _data + offset;
	T *src = _data + sz - 1, *dst = _data + sz + count - 1;

	while (src >= flag) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		--src, --dst;
	}
	for (size_type i = 0; i < count; ++i)
		std::allocator_traits<Alloc>::construct(alloc, flag++, value);

	sz += count;
	return begin() + offset;
}

template <typename T, typename Alloc> template <typename InputIt>
requires is_iterator<InputIt> vector<T, Alloc>::iterator
vector<T, Alloc>::insert(typename vector<T, Alloc>::const_iterator pos,
                         InputIt first, InputIt last) {
	size_type offset = pos - cbegin();
	size_type count  = last - first;

	if (sz + count > cap)
		_data = reallocate(sz + count);

	T *flag = _data + offset;
	T *src = _data + sz - 1, *dst = _data + sz + count - 1;

	while (src >= flag) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		--src, --dst;
	}
	while (first != last) {
		std::allocator_traits<Alloc>::construct(alloc, flag++, *(first++));
	}

	sz += count;
	return begin() + offset;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(typename vector<T, Alloc>::const_iterator pos,
                         std::initializer_list<T> ilist) {
	size_type offset = pos - cbegin();
	size_type count  = ilist.size();

	if (sz + count > cap)
		_data = reallocate(sz + count);

	T *flag = _data + offset;
	T *src = _data + sz - 1, *dst = _data + sz + count - 1;

	while (src >= flag) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		--src, --dst;
	}
	auto it = ilist.begin();
	while (it != ilist.end()) {
		std::allocator_traits<Alloc>::construct(alloc, flag++, *(it++));
	}

	sz += count;
	return begin() + offset;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::erase(vector<T, Alloc>::const_iterator pos) {
	size_type offset = pos - cbegin();

	T *src = _data + offset + 1;
	T *dst = _data + offset;

	std::allocator_traits<Alloc>::destroy(alloc, dst);

	while (src < _data + sz) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		++src, ++dst;
	}

	--sz;
	return begin() + offset;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::erase(vector<T, Alloc>::const_iterator first,
                        vector<T, Alloc>::const_iterator last) {
	size_type offset = first - cbegin();
	T *dst           = _data + (first - cbegin());
	T *src           = _data + (last - cbegin());

	T *walk = dst;
	while (walk != src) {
		std::allocator_traits<Alloc>::destroy(alloc, walk);
		++walk;
	}

	while (src < _data + sz) {
		std::allocator_traits<Alloc>::construct(alloc, dst, std::move(*src));
		++src, ++dst;
	}

	sz -= (last - first);
	return begin() + offset;
}
