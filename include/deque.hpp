#pragma once

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
	using iter = deque_iterator<T, Ref, ptr_rebind<Ptr, T>>;

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

	// TODO
	allocator_type get_allocator() const {}

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
		fill(value);
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
			T_alloc_traits::construct(impl, dst.operator->(), *src);
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

	bool empty() { return size() == 0; }

	size_type size() { return end() - begin(); }

	void clear() {
		destroy_elems(begin(), end());
		impl.finish = impl.start;
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
			T_alloc_traits::construct(impl, impl.finish.cur,
			                          std::move(value));
			++impl.finish.cur;
		} else {
			push_back_aux(value);
		}
	}

	void push_front(const T &value) {
		if (impl.start.cur > impl.start.first) {
			--impl.start.cur;
			T_alloc_traits::construct(impl, impl.start.cur, value);
		}
		else {
			push_front_aux(value);
		}
	}

	void push_front(T &&value) {
		if (impl.start.cur > impl.start.first) {
			--impl.start.cur;
			T_alloc_traits::construct(impl, impl.start.cur, std::move(value));
		}
		else {
			push_front_aux(value);
		}
	}

private:
	void fill(const T &value) {
		for (iterator it = impl.start; it != impl.finish; ++it)
			T_alloc_traits::construct(impl, it.operator->(), value);
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

		T_alloc_traits::construct(impl, impl.start.cur, std::forward<Args>(args)...);
	}
};
