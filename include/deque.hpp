#pragma once

#include "gtest/gtest.h"
#include <iostream>
#include <iterator.hpp>
#include <memory>

static constexpr size_t DEQUE_BUF_SIZE = 512;

inline size_t deque_buf_size(size_t size) {
	return size < DEQUE_BUF_SIZE ? size_t(DEQUE_BUF_SIZE / size) : size_t(1);
}

/**
 *  @brief A deque::iterator.
 *
 *  Quite a bit of intelligence here.  Much of the functionality of
 *  deque is actually passed off to this class.  A deque holds two
 *  of these internally, marking its valid range.  Access to
 *  elements is done as offsets of either of those two, relying on
 *  operator overloading in this class.
 *
 *  All the functions are op overloads except for _M_set_node.
 */
template <typename T, typename Ref, typename Ptr> class deque_iterator {
private:
	template <typename _Ptr, typename _T>
	using ptr_rebind = typename std::pointer_traits<_Ptr>::template rebind<_T>;
	template <typename CvTp>
	using iter = deque_iterator<T, CvTp &, ptr_rebind<Ptr, CvTp>>;
	/* template <typename CvTp>
	 * using iter = deque_iterator<T, Ref, ptr_rebind<Ptr, T>>; */

public:
	using iterator       = iter<T>;
	using const_iterator = iter<const T>;
	using elem_pointer   = ptr_rebind<Ptr, T>;
	using map_pointer    = ptr_rebind<Ptr, elem_pointer>;

	static size_t buffer_size() { return deque_buf_size(sizeof(T)); }

	using iterator_category = std::random_access_iterator_tag;
	using value_type        = T;
	using reference         = Ref;
	using pointer           = Ptr;
	using size_type         = size_t;
	using difference_type   = std::ptrdiff_t;

	elem_pointer cur;
	elem_pointer first;
	elem_pointer last;
	map_pointer node;

	deque_iterator() : cur(), first(), last(), node() {}

	deque_iterator(elem_pointer x, map_pointer y)
	    : cur(x), first(*y), last(*y + buffer_size()), node(y) {}

	// conversion from iterator to const_iterator
	template <typename Iter>
	requires std::is_same_v<deque_iterator, const_iterator> &&
	    std::is_same_v<Iter, iterator>
	    deque_iterator(const Iter &other)
	    : cur(other.cur), first(other.first), last(other.last),
	      node(other.node) {}

	deque_iterator(const deque_iterator &other)
	    : cur(other.cur), first(other.first), last(other.last),
	      node(other.node) {}

	deque_iterator &operator=(const deque_iterator &other) = default;

	reference operator*() { return *cur; }

	pointer operator->() { return cur; }

	deque_iterator &operator++() {
		++cur;
		if (cur == last) {
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}

	deque_iterator operator++(int) {
		deque_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	deque_iterator &operator--() {
		if (cur == first) {
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}

	deque_iterator operator--(int) {
		deque_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	deque_iterator &operator+=(difference_type n) {
		const difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size())) {
			cur += n;
		} else {
			const difference_type node_offset =
			    offset > 0
			        ? offset / difference_type(buffer_size())
			        : -difference_type((-offset - 1) / buffer_size()) - 1;

			set_node(node + node_offset);
			cur =
			    first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}

	deque_iterator &operator-=(difference_type n) { return *this += (-n); }

	reference operator[](difference_type n) const { return *(*this + n); }

	void set_node(map_pointer new_node) {
		node  = new_node;
		first = *node;
		last  = *node + difference_type(buffer_size());
	}

	friend bool operator==(const deque_iterator &lhs,
	                       const deque_iterator &rhs) {
		return lhs.cur == rhs.cur;
	}

	template <typename RefR, typename PtrR>
	friend bool operator==(const deque_iterator &lhs,
	                       const deque_iterator<T, RefR, PtrR> &rhs) {
		return lhs.cur == rhs.cur;
	}

	friend std::strong_ordering operator<=>(const deque_iterator &lhs,
	                                        const deque_iterator &rhs) {
		if (const auto cmp = (lhs.node <=> rhs.node); cmp != 0) {
			return cmp;
		}
		return lhs.cur <=> rhs.cur;
	}

	friend difference_type operator-(const deque_iterator &lhs,
	                                 const deque_iterator &rhs) {
		return buffer_size() * (lhs.node - rhs.node) + (lhs.cur - lhs.first) -
		       (rhs.cur - rhs.first);
	}

	template <typename RefR, typename PtrR>
	friend bool operator-(const deque_iterator &lhs,
	                      const deque_iterator<T, RefR, PtrR> &rhs) {
		return buffer_size() * (lhs.node - rhs.node) + (lhs.cur - lhs.first) -
		       (rhs.cur - rhs.first);
	}

	friend deque_iterator operator+(const deque_iterator &it,
	                                difference_type n) {
		deque_iterator ret = it;
		ret += n;
		return ret;
	}

	friend deque_iterator operator+(difference_type n,
	                                const deque_iterator &it) {
		return it + n;
	}

	friend deque_iterator operator-(const deque_iterator &it,
	                                difference_type n) {
		deque_iterator ret = it;
		ret -= n;
		return ret;
	}
};

/**
 *  Deque base class.  This class provides the unified face for %deque's
 *  allocation.  This class's constructor and destructor allocate and
 *  deallocate (but do not initialize) storage.  This makes %exception
 *  safety easier.
 *
 *  Nothing in this class ever constructs or destroys an actual Tp element.
 *  (Deque handles that itself.)  Only/All memory management is performed
 *  here.
 */
template <typename T, typename Alloc> class deque_base {
protected:
	using T_alloc_type = std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using T_alloc_traits =
	    std::allocator_traits<Alloc>::template rebind_traits<T>;

	using Ptr  = T_alloc_traits::pointer;
	using CPtr = T_alloc_traits::const_pointer;

	using map_alloc_type =
	    std::allocator_traits<Alloc>::template rebind_alloc<Ptr>;
	using map_alloc_traits =
	    std::allocator_traits<Alloc>::template rebind_traits<Ptr>;

	using allocator_type = Alloc;

	allocator_type get_allocator() const {
		return allocator_type(get_T_allocator());
	}

	using iterator       = deque_iterator<T, T &, Ptr>::iterator;
	using const_iterator = deque_iterator<T, const T &, CPtr>::iterator;

	deque_base() : impl() { init_map(0); }

	deque_base(size_t num_elems) : impl() { init_map(num_elems); }

	deque_base(const allocator_type &a) : impl(a) { init_map(0); }

	deque_base(const allocator_type &a, size_t num_elems) : impl(a) {
		init_map(num_elems);
	}

	deque_base(deque_base &&other) : impl(std::move(other.get_T_allocator())) {
		init_map(0);
		if (other.impl.map)
			impl.swap_data(other.impl);
	}

	deque_base(deque_base &&other, const allocator_type &a)
	    : impl(std::move(other.impl), a) {
		other.init_map(0);
	}

	deque_base(deque_base &&other, const allocator_type &a, size_t n)
	    : impl(a) {
		if (a == other.get_allocator()) {
			if (other.impl.map) {
				init_map(0);
				impl.swap_data(other.impl);
			}
		} else {
			init_map(n);
		}
	}

	using map_pointer = iterator::map_pointer;

	struct deque_impl_data {
		map_pointer map;
		size_t map_size;
		iterator start;
		iterator finish;

		deque_impl_data() : map(), map_size(0), start(), finish() {}

		deque_impl_data(const deque_impl_data &)            = default;
		deque_impl_data &operator=(const deque_impl_data &) = default;

		deque_impl_data(deque_impl_data &&other) : deque_impl_data(other) {
			other = deque_impl_data();
		}

		void swap_data(deque_impl_data &other) { std::swap(*this, other); }
	};

	struct deque_impl : public T_alloc_type, public deque_impl_data {
		deque_impl() : T_alloc_type() {}

		deque_impl(const T_alloc_type &a) : T_alloc_type(a) {}

		deque_impl(T_alloc_type &&a) : T_alloc_type(std::move(a)) {}

		deque_impl(deque_impl &&other) = default;

		deque_impl(deque_impl &&other, T_alloc_type &&a)
		    : T_alloc_type(a), deque_impl_data(std::move(other)) {}
	};

	T_alloc_type &get_T_allocator() { return impl; }

	const T_alloc_type &get_T_allocator() const { return impl; }

	map_alloc_type get_map_allocator() {
		return map_alloc_type(get_T_allocator());
	}

	Ptr allocate_node() {
		// using traits_type = std::allocator_traits<T_alloc_type>;
		return T_alloc_traits::allocate(impl, deque_buf_size(sizeof(T)));
	}

	void deallocate_node(Ptr p) {
		// using traits_type = std::allocator_traits<T_alloc_type>;
		T_alloc_traits::deallocate(impl, p, deque_buf_size(sizeof(T)));
	}

	map_pointer allocate_map(size_t n) {
		map_alloc_type map_alloc = get_map_allocator();
		return map_alloc_traits::allocate(map_alloc, n);
	}

	void deallocate_map(map_pointer p, size_t n) {
		map_alloc_type map_alloc = get_map_allocator();
		map_alloc_traits::deallocate(map_alloc, p, n);
	}

	void init_map(size_t);
	void create_nodes(map_pointer start, map_pointer finish);
	void destroy_nodes(map_pointer start, map_pointer finish);

	deque_impl impl;
};

// num_elems: T's count to allocate
template <typename T, typename Alloc>
void deque_base<T, Alloc>::init_map(size_t num_elems) {
	const size_t num_nodes = num_elems / deque_buf_size(sizeof(T)) + 1;
	impl.map_size          = num_nodes + 2;
	impl.map               = allocate_map(impl.map_size);

	// create_nodes(impl.map, impl.map + impl.map_size);

	map_pointer _start  = impl.map + 1;
	map_pointer _finish = _start + num_nodes;

	create_nodes(_start, _finish);

	impl.start.set_node(_start);
	impl.finish.set_node(_finish - 1);
	impl.start.cur = impl.start.first;
	impl.finish.cur =
	    impl.finish.first + (num_elems % deque_buf_size(sizeof(T)));
}

template <typename T, typename Alloc>
void deque_base<T, Alloc>::create_nodes(map_pointer start, map_pointer finish) {
	for (map_pointer cur = start; cur < finish; ++cur)
		*cur = allocate_node();
}

template <typename T, typename Alloc>
void deque_base<T, Alloc>::destroy_nodes(map_pointer start,
                                         map_pointer finish) {
	for (map_pointer cur = start; cur < finish; ++cur)
		deallocate_node(*cur);
}

template <typename T, typename Alloc = std::allocator<T>>
class deque : protected deque_base<T, Alloc> {
	using base           = deque_base<T, Alloc>;
	using T_alloc_type   = base::T_alloc_type;
	using T_alloc_traits = base::T_alloc_traits;
	using map_pointer    = base::map_pointer;

public:
	using value_type             = T;
	using reference              = value_type &;
	using const_reference        = const value_type &;
	using pointer                = T_alloc_traits::pointer;
	using const_pointer          = T_alloc_traits::const_pointer;
	using iterator               = base::iterator;
	using const_iterator         = base::const_iterator;
	using reverse_iterator       = tp::reverse_iterator<iterator>;
	using const_reverse_iterator = tp::reverse_iterator<const_iterator>;
	using size_type              = std::size_t;
	using difference_type        = std::ptrdiff_t;
	using allocator_type         = Alloc;

private:
	static size_type buffer_size() { return deque_buf_size(sizeof(T)); }
	using base::allocate_map;
	using base::allocate_node;
	using base::create_nodes;
	using base::deallocate_map;
	using base::deallocate_node;
	using base::destroy_nodes;
	using base::get_T_allocator;
	using base::init_map;

	using base::impl;

public:
	deque() = default;

	explicit deque(const Alloc &alloc) : base(alloc) {}

	explicit deque(size_type count, const Alloc alloc = Alloc())
	    : base(alloc, count) {}

	deque(size_type count, const T &value, const Alloc &alloc = Alloc())
	    : base(alloc, count) {
		initialize(impl.start, impl.finish, value);
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt> deque(InputIt first, InputIt last,
	                                        const Alloc &alloc = Alloc())
	    : base(alloc, last - first) {
		InputIt src  = first;
		iterator dst = begin();
		while (src != last) {
			T_alloc_traits::construct(impl, dst.operator->(), *src);
			++src, ++dst;
		}
	}

	// copy ctor
	deque(const deque &other)
	    : base(
	          T_alloc_traits::select_on_container_copy_construction(other.impl),
	          other.size()) {
		iterator src = other.begin();
		iterator dst = begin();
		while (src != other.end()) {
			T_alloc_traits::construct(impl, dst.operator->(), *src);
			++src, ++dst;
		}
	}

	deque(const deque &other, const Alloc &alloc) : base(alloc, other.size()) {
		iterator src = other.begin();
		iterator dst = begin();
		while (src != other.end()) {
			T_alloc_traits::construct(impl, dst.cur, *src);
			++src, ++dst;
		}
	}

	deque(deque &&other) : base(std::move(other)) {}

	deque(deque &&other, const Alloc &alloc) : base(std::move(other), alloc) {}

	deque(std::initializer_list<T> init, const Alloc &alloc = Alloc())
	    : base(alloc, init.size()) {
		auto src     = init.begin();
		iterator dst = begin();
		while (src != init.end()) {
			T_alloc_traits::construct(impl, dst.operator->(), *src);
			++src, ++dst;
		}
	}

	deque &operator=(const deque &other) {
		using pocca = T_alloc_traits::propagate_on_container_copy_assignment;
		if (std::addressof(other) != this) {
			if (pocca::value) {
				clear();
				deallocate_node(impl.start.first);

				get_T_allocator() = other.get_T_allocator();

				init_map(other.size());

				initialize_copy(other.begin(), other.end(), begin());

				return *this;
			}
			get_T_allocator() = other.get_T_allocator();
			size_type len     = size();
			if (len > other.size()) {
				erase_at_end(std::copy(other.begin(), other.end(), begin()));
			} else {
				auto mid = other.begin() + len;
				std::copy(other.begin(), mid, begin());
				insert_aux(end(), mid, other.end());
			}
		}
		return *this;
	}

	deque &operator=(deque &&other) {
		using pocma = T_alloc_traits::propagate_on_container_move_assignment;
		if (std::addressof(other) != this) {
			if (!pocma::value && get_T_allocator() != other.get_T_allocator()) {
				size_type len = size();
				if (len > other.size()) {
					erase_at_end(
					    std::move(other.begin(), other.end(), begin()));
				} else {
					auto mid = other.begin() + len;
					std::move(other.begin(), mid, begin());

					reserve_elems_at_back(other.size() - len);
					initialize_move(mid, other.end(), end());
				}
			} else {
				impl.swap_data(other.impl);
				get_T_allocator() = other.get_T_allocator();
			}
		}
		other.clear();
		return *this;
	}

	deque &operator=(std::initializer_list<T> ilist) {
		size_type len = size();
		if (len > ilist.size()) {
			erase_at_end(std::copy(ilist.begin(), ilist.end(), begin()));
		} else {
			auto mid = ilist.begin() + len;
			std::copy(ilist.begin(), mid, begin());
			insert_aux(end(), mid, ilist.end());
		}
		return *this;
	}

	void assign(size_type count, const T &value) {
		size_type len = size();
		if (len > count) {
			auto new_end = begin() + count;
			fill(begin(), new_end, value);
			erase_at_end(new_end);
		} else {
			fill(begin(), end(), value);
			insert_aux(end(), count - len, value);
		}
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt>
	void assign(InputIt first, InputIt last) {
		size_type len   = size();
		size_type count = last - first;
		if (len > count) {
			erase_at_end(std::copy(first, last, begin()));
		} else {
			auto mid = first + len;
			std::copy(first, mid, begin());
			insert_aux(end(), mid, last);
		}
	}

	void assign(std::initializer_list<T> ilist) {
		size_type len = size();
		if (len > ilist.size()) {
			erase_at_end(std::copy(ilist.begin(), ilist.end(), begin()));
		} else {
			auto mid = ilist.begin() + len;
			std::copy(ilist.begin(), mid, begin());
			insert_aux(end(), mid, ilist.end());
		}
	}

	allocator_type get_allocator() const { return base::get_allocator(); }

	~deque() {
		clear();
		deallocate_node(impl.start.first);
		deallocate_map(impl.map, impl.map_size);
	}

	reference at(size_type pos) {
		if (pos >= size())
			throw std::out_of_range("out of range");
		return *(begin() + pos);
	}

	const_reference at(size_type pos) const {
		if (pos >= size())
			throw std::out_of_range("out of range");
		return *(begin() + pos);
	}

	reference operator[](size_type pos) { return *(begin() + pos); }

	const_reference operator[](size_type pos) const { return *(begin() + pos); }

	reference front() { return *begin(); }

	const_reference front() const { return *begin(); }

	reference back() { return *(end() - 1); }

	const_reference back() const { return *(end() - 1); }

	iterator begin() { return impl.start; }

	const_iterator begin() const { return impl.start; }

	const_iterator cbegin() const { return impl.start; }

	iterator end() { return impl.finish; }

	const_iterator end() const { return impl.finish; }

	const_iterator cend() const { return impl.finish; }

	reverse_iterator rbegin() { return impl.finish - 1; }

	const_reverse_iterator rbegin() const { return impl.finish - 1; }

	const_reverse_iterator crbegin() const { return impl.finish - 1; }

	reverse_iterator rend() { return impl.start - 1; }

	const_reverse_iterator rend() const { return impl.start - 1; }

	const_reverse_iterator crend() const { return impl.start - 1; }

	bool empty() const { return size() == 0; }

	size_type size() const { return end() - begin(); }

	void shrink_to_fit() {
		// do nothing
	}

	void clear() { erase_at_end(begin()); }

	iterator insert(const_iterator pos, const T &value) {
		if (pos.cur == impl.start.cur) {
			push_front(value);
			return impl.start;
		} else if (pos.cur == impl.finish.cur) {
			push_back(value);
			iterator tmp = impl.finish;
			return --tmp;
		} else {
			return insert_aux(pos, value);
		}
	}

	iterator insert(const_iterator pos, T &&value) {
		if (pos.cur == impl.start.cur) {
			push_front(std::move(value));
			return impl.start;
		} else if (pos.cur == impl.finish.cur) {
			push_back(std::move(value));
			iterator tmp = impl.finish;
			return --tmp;
		} else {
			return insert_aux(pos, value);
		}
	}

	iterator insert(const_iterator pos, size_type count, const T &value) {
		if (pos.cur == impl.start.cur) {
			reserve_elems_at_front(count);
			iterator new_start = impl.start - count;
			initialize(new_start, impl.start, value);
			impl.start = new_start;
			return new_start;
		} else if (pos.cur == impl.finish.cur) {
			reserve_elems_at_back(count);
			iterator old_finish = impl.finish;
			impl.finish         = old_finish + count;
			initialize(old_finish, impl.finish, value);
			return old_finish;
		} else {
			return insert_aux(pos, count, value);
		}
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt>
	    iterator insert(const_iterator pos, InputIt first, InputIt last) {
		size_type count = static_cast<size_type>(last - first);
		if (pos.cur == impl.start.cur) {
			reserve_elems_at_front(count);
			iterator new_start = impl.start - count;
			initialize_copy(first, last, new_start);
			impl.start = new_start;
			return new_start;
		} else if (pos.cur == impl.finish.cur) {
			reserve_elems_at_back(count);
			iterator old_finish = impl.finish;
			impl.finish         = old_finish + count;
			initialize_copy(first, last, old_finish);
			return old_finish;
		} else {
			return insert_aux(pos, first, last);
		}
	}

	iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
		return insert(pos, ilist.begin(), ilist.end());
	}

	template <typename... Args>
	iterator emplace(const_iterator pos, Args &&...args) {
		if (pos.cur == impl.start.cur) {
			emplace_front(std::forward<Args>(args)...);
			return impl.start;
		} else if (pos.cur == impl.finish.cur) {
			emplace_back(std::forward<Args>(args)...);
			iterator ret = impl.finish - 1;
			return ret;
		} else {
			return insert_aux(pos, std::forward<Args>(args)...);
		}
	}

	iterator erase(const_iterator pos) {
		size_type index  = static_cast<size_type>(pos - begin());
		size_type length = size();
		iterator next    = pos;
		++next;
		if (index * 2 < length) {
			if (index != 0)
				transform_backward(impl.start, pos, next);
			pop_front();
		} else {
			if (index != length - 1)
				transform(next, impl.finish, pos);
			pop_back();
		}
		return begin() + index;
	}

	iterator erase(const_iterator first, const_iterator last) {
		if (first == last)
			return last;
		else if (first == begin() && last == end()) {
			clear();
			return end();
		} else {
			size_type count        = static_cast<size_type>(last - first);
			size_type elems_before = first - impl.start;

			if (elems_before * 2 < (size() - count)) {
				transform_backward(impl.start, first, last);
				erase_at_begin(impl.start + count);
			} else {
				transform(last, impl.finish, first);
				erase_at_end(impl.finish - count);
			}
			return impl.start + elems_before;
		}
	}

	void push_back(const T &value) {
		if (impl.finish.cur < impl.finish.last - 1) {
			T_alloc_traits::construct(impl, impl.finish.operator->(), value);
			++impl.finish.cur;
		} else {
			push_back_aux(value);
		}
	}

	void push_back(T &&value) {
		if (impl.finish.cur < impl.finish.last - 1) {
			T_alloc_traits::construct(impl, impl.finish.cur, std::move(value));
			++impl.finish.cur;
		} else {
			push_back_aux(value);
		}
	}

	void pop_back() {
		if (impl.finish.cur != impl.finish.first) {
			--impl.finish;
			T_alloc_traits::destroy(impl, impl.finish.cur);
		} else {
			deallocate_node(impl.finish.first);
			impl.finish.set_node(impl.finish.node - 1);
			impl.finish.cur = impl.finish.last;
			T_alloc_traits::destroy(impl, impl.finish.first);
		}
	}

	template <typename... Args> reference emplace_back(Args &&...args) {
		if (impl.finish.cur < impl.finish.last - 1) {
			T_alloc_traits::construct(impl, impl.finish.cur,
			                          std::forward<Args>(args)...);

			++impl.finish.cur;
		} else {
			push_back_aux(std::forward<Args>(args)...);
		}
		return *(impl.finish.cur - 1);
	}

	void push_front(const T &value) {
		if (impl.start.cur > impl.start.first) {
			--impl.start.cur;
			T_alloc_traits::construct(impl, impl.start.cur, value);
		} else {
			push_front_aux(value);
		}
	}

	void push_front(T &&value) {
		if (impl.start.cur > impl.start.first) {
			--impl.start.cur;
			T_alloc_traits::construct(impl, impl.start.cur, std::move(value));
		} else {
			push_front_aux(value);
		}
	}

	void pop_front() {
		if (impl.start.cur != impl.start.last - 1) {
			T_alloc_traits::destroy(impl, impl.start.cur);
			++impl.start;
		} else {
			T_alloc_traits::destroy(impl, impl.start.cur);
			deallocate_node(impl.start.first);
			impl.start.set_node(impl.start.node + 1);
			impl.start.cur = impl.start.first;
		}
	}

	template <typename... Args> reference emplace_front(Args &&...args) {
		if (impl.start.cur > impl.start.first) {
			--impl.start.cur;
			T_alloc_traits::construct(impl, impl.start.cur,
			                          std::forward<Args>(args)...);
		} else {
			push_front_aux(std::forward<Args>(args)...);
		}
		return *(impl.start.cur);
	}

	void resize(size_type count) {
		size_type length = size();
		if (count == length)
			return;
		if (count > length)
			insert(end(), count - length, value_type());
		else
			erase(begin() + count, end());
	}

	void resize(size_type count, const value_type &value) {
		size_type length = size();
		if (count == length)
			return;
		if (count > length)
			insert(end(), count - length, value);
		else
			erase(begin() + count, end());
	}

	void swap(deque &other) {
		using pocs = T_alloc_traits::propagate_on_container_swap;

		assert(pocs::value || get_T_allocator() == other.get_T_allocator());

		impl.swap_data(other.impl);

		if (pocs::value) {
			using std::swap;
			swap(get_T_allocator(), other.get_T_allocator());
		}
	}

private:
	void fill(const T &value) {
		for (iterator it = impl.start; it != impl.finish; ++it)
			T_alloc_traits::construct(impl, it.operator->(), value);
	}

	void fill(const_iterator first, const_iterator last, const T &value) {
		auto it = first;
		while (it != last) {
			*it = value;
			++it;
		}
	}

	void destroy_elem(iterator &pos) {
		T_alloc_traits::destroy(impl, pos.operator->());
	}

	void destroy_elems(const iterator &first, const iterator &last) {
		for (auto it = first; it != last; ++it)
			destroy_elem(it);
	}

	void reallocate_map(size_type cnt, bool add_at_front) {
		size_type old_num_nodes = impl.finish.node - impl.start.node + 1;
		size_type new_num_nodes = old_num_nodes + cnt;

		size_type new_map_size =
		    impl.map_size + std::max(impl.map_size, cnt) + 2;
		map_pointer new_map   = allocate_map(new_map_size);
		map_pointer new_start = new_map + (new_map_size - new_num_nodes) / 2 +
		                        (add_at_front ? cnt : 0);

		std::copy(impl.start.node, impl.finish.node + 1, new_start);

		deallocate_map(impl.map, impl.map_size);

		impl.map      = new_map;
		impl.map_size = new_map_size;

		size_type start_offset  = impl.start.cur - impl.start.first;
		size_type finish_offset = impl.finish.cur - impl.finish.first;
		impl.start.set_node(new_start);
		impl.finish.set_node(new_start + old_num_nodes - 1);
		impl.start.cur  = impl.start.first + start_offset;
		impl.finish.cur = impl.finish.first + finish_offset;
	}

	void reserve_elems_at_front(size_type n) {
		size_type free_elems_cnt = impl.start.cur - impl.start.first;
		if (free_elems_cnt < n)
			new_elems_at_front(n - free_elems_cnt);
	}

	void new_elems_at_front(size_type n) {
		const size_type new_nodes = (n + buffer_size() - 1) / buffer_size();
		reserve_map_at_front(new_nodes);

		for (size_type i = 1; i <= new_nodes; ++i)
			*(impl.start.node - i) = allocate_node();
	}

	void reserve_elems_at_back(size_type n) {
		size_type free_elems_cnt = impl.finish.last - impl.finish.cur;
		if (free_elems_cnt < n)
			new_elems_at_back(n - free_elems_cnt);
	}

	void new_elems_at_back(size_type n) {
		const size_type new_nodes = (n + buffer_size() - 1) / buffer_size();
		reserve_map_at_back(new_nodes);

		for (size_type i = 1; i <= new_nodes; ++i)
			*(impl.finish.node + i) = allocate_node();
	}

	size_type free_node_cnt_at_front() {
		return size_type(impl.start.node - impl.map);
	}

	size_type free_node_cnt_at_back() {
		return size_type(impl.map + impl.map_size - 1 - impl.finish.node);
	}

	void reserve_map_at_front(size_type nodes_to_add = 1) {
		if (free_node_cnt_at_front() < nodes_to_add)
			reallocate_map(nodes_to_add, true);
	}

	void reserve_map_at_back(size_type nodes_to_add = 1) {
		if (free_node_cnt_at_back() < nodes_to_add)
			reallocate_map(nodes_to_add, false);
	}

	template <typename... Args> void push_back_aux(Args &&...args) {
		reserve_map_at_back();
		*(impl.finish.node + 1) = allocate_node();
		T_alloc_traits::construct(impl, impl.finish.cur,
		                          std::forward<Args>(args)...);

		impl.finish.set_node(impl.finish.node + 1);
		impl.finish.cur = impl.finish.first;
	}

	template <typename... Args> void push_front_aux(Args &&...args) {
		reserve_map_at_front();
		*(impl.start.node - 1) = allocate_node();
		impl.start.set_node(impl.start.node - 1);
		impl.start.cur = impl.start.last - 1;

		T_alloc_traits::construct(impl, impl.start.cur,
		                          std::forward<Args>(args)...);
	}

	template <typename... Args>
	iterator insert_aux(const_iterator pos, Args &&...args) {
		value_type value(std::forward<Args>(args)...);
		difference_type index = pos - impl.start;

		if (static_cast<size_type>(index) * 2 < size()) {
			push_front(std::move(front()));

			iterator front1 = impl.start;
			++front1;
			iterator front2 = front1;
			++front2;
			pos = impl.start +
			      index; // because iterator maybe invalid after push_front
			iterator pos1 = pos;
			++pos1;
			transform(front2, pos1, front1);
		} else {
			push_back(std::move(back()));
			iterator back1 = impl.finish;
			--back1;
			iterator back2 = back1;
			--back2;
			pos = impl.start + index;

			transform_backward(pos, back2, back1);
		}
		*pos = std::move(value);
		return pos;
	}

	iterator insert_aux(const_iterator pos, size_type count, const T &value) {
		size_type elems_before = pos - impl.start;
		size_type length       = size();

		if (elems_before * 2 < length) {
			reserve_elems_at_front(count);
			iterator new_start = impl.start - count;
			iterator old_start = impl.start;
			pos                = impl.start + elems_before;
			initialize_move(old_start, pos, new_start);
			initialize(new_start + elems_before, pos, value);
			impl.start = new_start;
		} else {
			reserve_elems_at_back(count);
			iterator new_finish = impl.finish + count;
			iterator old_finish = impl.finish;
			pos                 = impl.start + elems_before;
			initialize_move_backward(pos, old_finish, new_finish);
			initialize(pos, pos + count, value);
			impl.finish = new_finish;
		}
		return impl.start + elems_before;
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt>
	    iterator insert_aux(const_iterator pos, InputIt first, InputIt last) {
		size_type elems_before = pos - impl.start;
		size_type length       = size();
		size_type count        = last - first;

		if (elems_before * 2 < length) {
			reserve_elems_at_front(count);
			iterator new_start = impl.start - count;
			iterator old_start = impl.start;
			pos                = impl.start + elems_before;
			initialize_move(old_start, pos, new_start);
			initialize_copy(first, last, new_start + elems_before);
			impl.start = new_start;
		} else {
			reserve_elems_at_back(count);
			iterator new_finish = impl.finish + count;
			iterator old_finish = impl.finish;
			pos                 = impl.start + elems_before;
			initialize_move_backward(pos, old_finish, new_finish);
			initialize_copy(first, last, pos);
			impl.finish = new_finish;
		}
		return impl.start + elems_before;
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt>
	    iterator assign_aux(const_iterator pos, InputIt first, InputIt last) {
		size_type elems_before = pos - impl.start;
		size_type length       = size();
		size_type count        = last - first;

		if (elems_before * 2 < length) {
			reserve_elems_at_front(count);
			iterator new_start = impl.start - count;
			iterator old_start = impl.start;
			pos                = impl.start + elems_before;
			initialize_move(old_start, pos, new_start);
			initialize_move(first, last, new_start + elems_before);
			impl.start = new_start;
		} else {
			reserve_elems_at_back(count);
			iterator new_finish = impl.finish + count;
			iterator old_finish = impl.finish;
			pos                 = impl.start + elems_before;
			initialize_move_backward(pos, old_finish, new_finish);
			initialize_move(first, last, pos);
			impl.finish = new_finish;
		}
		return impl.start + elems_before;
	}

	void erase_at_begin(iterator pos) {
		destroy_elems(begin(), pos);
		destroy_nodes(impl.start.node, pos.node);
		impl.start = pos;
	}

	void erase_at_end(iterator pos) {
		destroy_elems(pos, end());
		destroy_nodes(pos.node + 1, impl.finish.node + 1);
		impl.finish = pos;
	}

	void transform(iterator src_first, iterator src_last, iterator dest_first) {
		auto it = src_first;
		while (it != src_last) {
			*dest_first = std::move(*it);
			++it, ++dest_first;
		}
	}

	void transform_backward(iterator src_first, iterator src_last,
	                        iterator dest_last) {
		auto it = src_last;
		--it, --dest_last;

		while (it >= src_first) {
			*dest_last = std::move(*it);
			--it, --dest_last;
		}
	}

	template <typename InputIt, typename... Args>
	requires tp::is_iterator<InputIt>
	void initialize(InputIt first, InputIt last, Args &&...args) {
		auto it = first;
		while (it != last) {
			T_alloc_traits::construct(impl, it.cur,
			                          std::forward<Args>(args)...);
			++it;
		}
	}

	template <typename InputItL, typename InputItR>
	requires tp::is_iterator<InputItL> && tp::is_iterator<InputItR>
	void initialize_copy(InputItL src_first, InputItL src_last,
	                     InputItR dest_first) {
		auto it = src_first;
		while (it != src_last) {
			T_alloc_traits::construct(impl, dest_first.cur, *it);
			++it, ++dest_first;
		}
	}

	template <typename InputItL, typename InputItR>
	requires tp::is_iterator<InputItL> && tp::is_iterator<InputItR>
	void initialize_copy_backward(InputItL src_first, InputItL src_last,
	                              InputItR dest_last) {
		auto it = src_last;
		--it, --dest_last;
		while (it >= src_first) {
			T_alloc_traits::construct(impl, dest_last.cur, *it);
			--it, --dest_last;
		}
	}

	template <typename InputItL, typename InputItR>
	requires tp::is_iterator<InputItL> && tp::is_iterator<InputItR>
	void initialize_move(InputItL src_first, InputItL src_last,
	                     InputItR dest_first) {
		auto it = src_first;
		while (it != src_last) {
			T_alloc_traits::construct(impl, dest_first.cur, std::move(*it));
			// *dest_first = std::move(*it);
			++it, ++dest_first;
		}
	}

	template <typename InputItL, typename InputItR>
	requires tp::is_iterator<InputItL> && tp::is_iterator<InputItR>
	void initialize_move_backward(InputItL src_first, InputItL src_last,
	                              InputItR dest_last) {
		auto it = src_last;
		--it, --dest_last;
		while (it >= src_first) {
			T_alloc_traits::construct(impl, dest_last.cur, std::move(*it));
			--it, --dest_last;
		}
	}

	template <typename... Args> void replace_map(Args &&...args) {
		deque new_deque(std::forward<Args>(args)...);
		clear();
		deallocate_node(*impl.start.node);
		deallocate_map(impl.map, impl.map_size);

		impl.map      = nullptr;
		impl.map_size = 0;
		impl.swap_data(new_deque.impl);
	}
};
