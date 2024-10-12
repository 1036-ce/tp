#pragma once

#include <cstddef>
#include <iterator.hpp>
#include <memory>

struct list_node_base {
	list_node_base *_next;
	list_node_base *_prev;

	// add [first, last) before this
	void transfer(list_node_base *first, list_node_base *last) {
		list_node_base *tmp = last->_prev;

		last->_prev         = first->_prev;
		first->_prev->_next = last;

		first->_prev       = this->_prev;
		this->_prev->_next = first;
		this->_prev        = tmp;
		tmp->_next         = this;
	}

	void reverse() {
		list_node_base *tmp;

		list_node_base *node = this->_next;
		while (node != this) {
			tmp         = node->_next;
			node->_next = node->_prev;
			node->_prev = tmp;
			node        = tmp;
		}
		tmp         = this->_next;
		this->_next = this->_prev;
		this->_prev = tmp;
	}

	// this->_next = node, node->_prev = this;
	void hook(list_node_base* const node) {
		node->_prev->_next = this;
		this->_prev = node->_prev;
		node->_prev = this;
		this->_next = node;
	}

	void unhook() {
		this->_prev->_next = this->_next;
		this->_next->_prev = this->_prev;
	}
};

struct list_node_header : public list_node_base {
	list_node_header() { init(); }

	list_node_header(list_node_header &&other)
	    : list_node_base{other._next, other._prev}, _size(other._size) {
		if (other._base()->_next == other._base())
			this->_next = this->_prev = this;
		else {
			this->_prev->_next = this->_next->_prev = this;
			other.init();
		}
	}

	void move_nodes(list_node_header &&other) {
		if (other._base()->_next == other._base())
			init();
		else {
			this->_next        = other._next;
			this->_prev        = other._prev;
			this->_next->_prev = this->_prev->_next = this;
			this->_size                             = other._size;
			other.init();
		}
	}

	void init() {
		this->_next = this->_prev = this;
		_size                     = 0;
	}

	list_node_base *_base() { return this; }

	std::size_t _size;
};

class scratch_list : public list_node_base {
public:
private:
};

template <typename T> class list_node : public list_node_base {
public:
	T *ptr() { return &val; }

	const T *cptr() { return &val; }

private:
	T val;
};

template <typename T> struct list_iterator {
	using Node              = list_node<T>;
	using value_type        = T;
	using pointer           = T *;
	using reference         = T &;
	using iterator_category = std::bidirectional_iterator_tag;

	list_iterator() : node() {}

	list_iterator(list_node_base *nd) : node(nd) {}

	reference operator*() { return *(static_cast<Node *>(node)->ptr()); }

	pointer operator->() { return static_cast<Node *>(node)->ptr(); }

	list_iterator &operator++() {
		node = node->_next;
		return *this;
	}

	list_iterator operator++(int) {
		list_iterator ret = *this;
		node              = node->_next;
		return ret;
	}

	list_iterator &operator--() {
		node = node->_prev;
		return *this;
	}

	list_iterator operator--(int) {
		list_iterator ret = *this;
		node              = node->_prev;
		return ret;
	}

	friend bool operator==(const list_iterator &lhs, const list_iterator &rhs) {
		return (lhs.node == rhs.node);
	}

	list_node_base *node;
};

template <typename T> struct list_const_iterator {
	using Node              = list_node<T>;
	using iterator          = list_iterator<T>;
	using value_type        = T;
	using pointer           = const T *;
	using reference         = const T &;
	using iterator_category = std::bidirectional_iterator_tag;

	list_const_iterator() : node() {}

	list_const_iterator(list_node_base *nd) : node(nd) {}

	list_const_iterator(const iterator &other) : node(other.node) {}

	reference operator*() { return *(static_cast<Node *>(node)->ptr()); }

	pointer operator->() { return static_cast<Node *>(node)->ptr(); }

	list_const_iterator &operator++() {
		node = node->_next;
		return *this;
	}

	list_const_iterator operator++(int) {
		list_iterator ret = *this;
		node              = node->_next;
		return ret;
	}

	list_const_iterator &operator--() {
		node = node->_prev;
		return *this;
	}

	list_const_iterator operator--(int) {
		list_iterator ret = *this;
		node              = node->_prev;
		return ret;
	}

	friend bool operator==(const list_const_iterator &lhs,
	                       const list_const_iterator &rhs) {
		return (lhs.node == rhs.node);
	}

	iterator remove_const() {
		return iterator(const_cast<list_node_base*>(node));
	} 

	// TODO
	// should be : `const list_node_base *node`
	const list_node_base *node;
};

template <typename T, typename Alloc> class list_base {
protected:
	using Node         = list_node<T>;
	using T_alloc_type = std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using T_alloc_traits =
	    std::allocator_traits<Alloc>::template rebind_traits<T>;
	using Node_alloc_type =
	    std::allocator_traits<Alloc>::template rebind_alloc<Node>;
	using Node_alloc_traits =
	    std::allocator_traits<Alloc>::template rebind_traits<Node>;

	static std::size_t distance(list_node_base *first, list_node_base *last) {
		list_node_base *node = first;
		std::size_t ret      = 0;
		while (node != last) {
			node = node->_next;
			++ret;
		}
		return ret;
	}

	struct list_impl : public Node_alloc_type {
		list_node_header header;

		list_impl() : Node_alloc_type() {}

		list_impl(const Node_alloc_type &a) : Node_alloc_type(a) {}

		list_impl(Node_alloc_type &&a) : Node_alloc_type(std::move(a)) {}

		list_impl(list_impl &&) = default;

		list_impl(Node_alloc_type &&a, list_impl &&other)
		    : Node_alloc_type(a), header(std::move(other.header)) {}
	};

	list_impl impl;

	std::size_t get_size() const { return impl.header._size; }

	void set_size(std::size_t new_size) { impl.header._size = new_size; }

	void inc_size(std::size_t n) { impl.header._size += n; }

	void dec_size(std::size_t n) { --impl.header._size -= n; }

	std::size_t node_count() const { return get_size(); }

	Node_alloc_traits::pointer allocate_node() {
		return Node_alloc_traits::allocate(impl, 1);
	}

	void deallocate_node(Node_alloc_traits::pointer ptr) {
		Node_alloc_traits::deallocate(impl, ptr, 1);
	}

	Node_alloc_type &get_Node_allocator() { return impl; }

	const Node_alloc_type &get_Node_allocator() const { return impl; }

	list_base() = default;

	list_base(const Node_alloc_type &a) : impl(a) {}

	// move ctor
	list_base(list_base &&) = default;

	list_base(list_base &&other, Node_alloc_type &&a) : impl(std::move(a)) {
		if (other.get_Node_allocator() == get_Node_allocator())
			move_nodes(std::move(other));
		// else **caller** must move individual elements.
	}

	// used when allocator is_always_equal
	list_base(Node_alloc_type &&a, list_base &&other)
	    : impl(std::move(a), std::move(other.impl)) {}

	// used when allocator !is_always_equal
	list_base(Node_alloc_type &&a) : impl(std::move(a)) {}

	~list_base() { clear(); }

	void move_nodes(list_base &&other) {
		impl.header.move_nodes(std::move(other.impl.header));
	}

	void clear() {
		list_node_base *cur = impl.header._next;

		while (cur != impl.header._base()) {
			Node *tmp = static_cast<Node *>(cur);
			cur       = tmp->_next;

			T *valp = tmp->ptr();
			Node_alloc_traits::destroy(get_Node_allocator(), valp);

			deallocate_node(tmp);
		}
	}

	void init() { impl.header.init(); }
};

template <typename T, typename Alloc = std::allocator<T>>
class list : protected list_base<T, Alloc> {
	using base              = list_base<T, Alloc>;
	using T_alloc_type      = base::T_alloc_type;
	using T_alloc_traits    = base::T_alloc_traits;
	using Node_alloc_type   = base::Node_alloc_type;
	using Node_alloc_traits = base::Node_alloc_traits;

public:
	using value_type             = T;
	using allocator_type         = Alloc;
	using size_type              = std::size_t;
	using difference_type        = std::ptrdiff_t;
	using reference              = value_type &;
	using const_reference        = const value_type &;
	using pointer                = T_alloc_traits::pointer;
	using const_pointer          = T_alloc_traits::const_pointer;
	using iterator               = list_iterator<T>;
	using const_iterator         = list_const_iterator<T>;
	using reverse_iterator       = tp::reverse_iterator<iterator>;
	using const_reverse_iterator = tp::reverse_iterator<const_iterator>;

protected:
	using Node = list_node<T>;
	using base::allocate_node;
	using base::deallocate_node;
	using base::get_Node_allocator;
	using base::impl;

	template <typename... Args> Node *create_node(Args &&...args) {
		Node *ret   = allocate_node();
		auto &alloc = get_Node_allocator();
		Node_alloc_traits::construct(alloc, ret->ptr(),
		                             std::forward<Args>(args)...);
		return ret;
	}

public:
	list() = default;

	explicit list(const Alloc &alloc) : base(Node_alloc_type(alloc)) {}

	list(size_type count, const T &value, const Alloc &alloc = Alloc())
	    : base(Node_alloc_type(alloc)) {
		for (size_type i = 0; i < count; ++i)
			push_back(value);
	}

	explicit list(size_type count, const Alloc &alloc = Alloc())
	    : base(Node_alloc_type(alloc)) {
		for (size_type i = 0; i < count; ++i)
			emplace_back();
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt> list(InputIt first, InputIt last,
	                                       const Alloc &alloc = Alloc())
	    : base(Node_alloc_type(alloc)) {
		auto it = first;
		while (it != last) {
			emplace_back(*first);
			++it;
		}
	}

	// copy ctor
	list(const list &other)
	    : base(Node_alloc_traits::select_on_container_copy_construction(
	          other.get_Node_allocator())) {
		initialize(other.begin(), other.end());
	}

	// allocator-extented copy ctor
	list(const list &other, const Alloc &alloc) : base(alloc) {
		initialize(other.begin(), other.end());
	}

	// move ctor
	list(list &&other) = default;

private:
	list(list &&other, const Alloc &alloc, std::true_type)
	    : base(std::move(alloc), std::move(other)) {}

	list(list &&other, const Alloc &alloc, std::false_type)
	    : base(std::move(alloc)) {
		if (other.get_Node_allocator() == this->get_Node_allocator()) {
			this->move_nodes(other);
		} else {
			// TODO
		}
	}

public:
	// allocator-extented move ctor
	list(list &&other, const Alloc &alloc)
	    : list(std::move(other), alloc, Node_alloc_traits::is_always_equal) {}

	list(std::initializer_list<T> init, const Alloc &alloc = Alloc())
	    : base(Node_alloc_type(alloc)) {
		auto it = init.begin();
		while (it != init.end()) {
			emplace_back(*it);
			++it;
		}
	}

	allocator_type get_allocator() const {
		return allocator_type(base::get_Node_allocator());
	}

	iterator begin() { return iterator(impl.header._next); }

	const_iterator begin() const { return const_iterator(impl.header._next); }

	const_iterator cbegin() const { return const_iterator(impl.header._next); }

	iterator end() { return iterator(&impl.header); }

	const_iterator end() const { return const_iterator(&impl.header); }

	const_iterator cend() const { return const_iterator(&impl.header); }

	reverse_iterator rbegin() {
		return reverse_iterator(iterator(impl.header._prev));
	}

	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(iterator(impl.header._prev));
	}

	const_reverse_iterator crbegin() const {
		return const_reverse_iterator(iterator(impl.header._prev));
	}

	reverse_iterator rend() { return reverse_iterator(iterator(&impl.header)); }

	const_reverse_iterator rend() const {
		return const_reverse_iterator(iterator(&impl.header));
	}

	const_reverse_iterator crend() const {
		return const_reverse_iterator(iterator(&impl.header));
	}

	bool empty() const { return impl.header._next == &impl.header; }

	size_type size() const { return base::get_size(); }

	void clear() {
		base::clear();
		base::init();
	}

	iterator insert(const_iterator pos, const T& value) {
		return emplace(pos, value);
	}

	iterator insert(const_iterator pos, T&& value) {
		return emplace(pos, std::move(value));
	}

	iterator insert(const_iterator pos, size_type count, const T& value) {
		iterator ret;
		while (count--)
			ret = emplace(pos, value);
		return ret;
	}

	template <typename InputIt>
		requires tp::is_iterator<InputIt>
	iterator insert(const_iterator pos, InputIt first, InputIt last) {
		auto it = first;
		iterator ret;
		while (it != last) {
			ret = emplace(pos, *it);
			++it;
		}
		return ret;
	}

	iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
		return insert(pos, ilist.begin(), ilist.end());
	}

	iterator erase(const_iterator pos) {
		iterator ret(pos.node->_next);
		erase_aux(pos.remove_const());
		return ret;
	}

	iterator erase(const_iterator first, const_iterator last) {
		while (first != last) {
			first = erase(first);
		}
		return last.remove_const();
	}

	template <typename... Args>
	iterator emplace(const_iterator pos, Args &&...args) {
		Node *node = create_node(std::forward<Args>(args)...);
		node->hook(pos.remove_const().node);
		base::inc_size(1);
		return iterator(node);
	}

	void push_back(const T &value) {
		Node *node = create_node(value);
		insert_at_end(node);
	}

	void push_back(T &&value) {
		Node *node = create_node(value);
		insert_at_end(node);
	}

	template <typename... Args> reference emplace_back(Args &&...args) {
		Node *node = create_node(std::forward<Args>(args)...);
		insert_at_end(node);
		return *node->ptr();
	}

	void pop_back() {
		erase_aux(--end());
	}

	void push_front(const T &value) {
		Node *node = create_node(value);
		insert_at_begin(node);
	}

	void push_front(T&& value) {
		Node *node = create_node(value);
		insert_at_begin(node);
	}

	template <typename... Args>
		reference emplace_front(Args&&... args) {
			Node *node = create_node(std::forward<Args>(args)...);
			insert_at_begin(node);
			return *node->ptr();
		}

	void pop_front() {
		erase_aux(begin());
	}

private:
	void insert_at_begin(Node *node) {
		Node *first = static_cast<Node*>(impl.header._next);
		node->_prev = &(impl.header);
		node->_next = first;

		first->_prev = node;
		impl.header._next = node;
		base::inc_size(1);
	}

	void insert_at_end(Node *node) {
		Node *last  = static_cast<Node *>(impl.header._prev);
		node->_next = &(impl.header);
		node->_prev = impl.header._prev;

		last->_next       = node;
		impl.header._prev = node;
		base::inc_size(1);
	}

	template <typename InputIt>
	requires tp::is_iterator<InputIt>
	void initialize(InputIt first, InputIt last) {
		auto it = first;

		while (it != last) {
			insert_at_end(create_node(*it));
			++it;
		}
	}

	void erase_aux(iterator pos) {
		base::dec_size(1);
		pos.node->unhook();
		Node *node = static_cast<Node*>(pos.node);
		Node_alloc_traits::destroy(get_Node_allocator(), node->ptr());
		deallocate_node(node);
	}
};
