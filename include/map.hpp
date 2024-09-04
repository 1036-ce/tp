// based on rbtree
#pragma once

#include <cstdio>
#include <initializer_list>
#include <rbtree_impl.hpp>
#include <utility>

namespace tp {

template <typename T1, typename T2> struct pair {
	using first_type  = T1;
	using second_type = T2;

	pair() = default;
	pair(const first_type &_first, const second_type &_second)
	    : first(_first), second(_second) {}
	pair(first_type &&_first, second_type &&_second)
	    : first(_first), second(_second) {}
	template <typename U1, typename U2>
	pair(U1 &&x, U2 &&y) : first(x), second(y) {}

	template <typename U1, typename U2>
	pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}

	pair(const pair &other) : first(other.first), second(other.second) {}
	pair(pair &&other) : first(other.first), second(other.second) {}

	pair &operator=(const pair &other) {
		first  = other.first;
		second = other.second;
		return *this;
	}

	pair &operator=(pair &&other) {
		first  = other.first;
		second = other.second;
		return *this;
	}

	~pair() = default;

	first_type first{};
	second_type second{};
};

template <typename T> struct less {
	bool operator()(const T &lhs, const T &rhs) const { return lhs < rhs; }
};

template <typename T1, typename T2> pair<T1, T2> make_pair(T1 t, T2 u) {
	return pair<T1, T2>(t, u);
}

template <typename Key, typename T, typename Comp = less<Key>> class map {
private:
	struct node;
	struct iterator;
	struct const_iterator;
	struct reverse_iterator;
	struct const_reverse_iterator;

public:
	using key_type               = Key;
	using mapped_type            = T;
	using value_type             = pair<const key_type, mapped_type>;
	using size_type              = std::size_t;
	using reference              = value_type &;
	using const_reference        = const value_type &;
	using iterator               = iterator;
	using const_iterator         = const_iterator;
	using reverse_iterator       = reverse_iterator;
	using const_reverse_iterator = const_reverse_iterator;
	// TODO: node_type should change to shared_ptr, otherwise it will leak memory
	using node_type              = node;

	map() = default;
	map(std::initializer_list<value_type> init, const Comp &_comp = Comp());
	~map();

	mapped_type &at(const key_type &key);
	const mapped_type &at(const key_type &key) const;

	mapped_type &operator[](const key_type &key);
	mapped_type &operator[](key_type &&key);

	// iterators
	iterator begin() {
		node_type *nd = rb_entry_safe(rb_first(rbr.node), node_type, rbn);
		return iterator(nd);
	}
	iterator end() { return iterator(nullptr); }

	const_iterator cbegin() const {
		node_type *nd = rb_entry_safe(rb_first(rbr.node), node_type, rbn);
		return const_iterator(nd);
	}
	const_iterator cend() const { return const_iterator(nullptr); }

	reverse_iterator rbegin() {
		node_type *nd = rb_entry_safe(rb_last(rbr.node), node_type, rbn);
		return reverse_iterator(nd);
	}
	reverse_iterator rend() { return reverse_iterator(nullptr); }

	const_reverse_iterator crbegin() const {
		node_type *nd = rb_entry_safe(rb_last(rbr.node), node_type, rbn);
		return const_reverse_iterator(nd);
	}
	const_reverse_iterator crend() const {
		return const_reverse_iterator(nullptr);
	}

	// capacity
	bool empty() { return _size == 0; }

	size_type size() { return _size; }

	// TODO: max_size()

	// modifiers
	void clear();

	pair<iterator, bool> insert(const value_type &value);
	pair<iterator, bool> insert(value_type &&value);

	template <typename... Args> pair<iterator, bool> emplace(Args &&...args);

	iterator erase(iterator pos);

	// lookup
	size_type count(const key_type &key) const;

	iterator find(const key_type &key);

	// returns an iterator to the first element **not less** than the given key
	iterator lower_bound(const key_type &key);
	const_iterator lower_bound(const key_type &key) const;

	// returns an iterator to the first element **greater** than the given key
	iterator upper_bound(const key_type &key);
	const_iterator upper_bound(const key_type &key) const;

private:
	node_type *create_node(const value_type &value);
	void destroy_node(node_type *nd);
	node_type *find_node(const key_type &key) const;
	void insert_node(node_type *nd);
	// insert value into map, if key already exists, return this node
	// otherwise create a new node and return it;
	node_type *insert_value(const value_type &value);

	struct node {
		node() = default;
		node(const value_type &_value) : value(_value), has_value(true) {}
		node(value_type &&_value) : value(_value), has_value(true) {}

		const key_type &key() { return value.first; }
		mapped_type &mapped() { return value.second; }
		bool empty() { return !has_value; }

		rbnode rbn{};
		value_type value{};
		bool has_value{false};
	};

	struct iterator {
		friend class map;
		iterator(node_type *_nd) : nd(_nd) {}

		bool operator==(const iterator &other) const {
			return other.nd == this->nd;
		}

		bool operator!=(const iterator &other) const {
			return !(other == *this);
		}

		iterator &operator=(const iterator &other) {
			nd = other.nd;
			return *this;
		}

		// ++it
		iterator &operator++() {
			rbnode *nxt = rb_next(&(nd->rbn));
			nd          = rb_entry_safe(nxt, node_type, rbn);
			return *this;
		}

		// it++;
		iterator operator++(int) {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		// --it
		iterator &operator--() {
			rbnode *nxt = rb_prev(&(nd->rbn));
			nd          = rb_entry_safe(nxt, node_type, rbn);
			return *this;
		}

		// it--
		iterator operator--(int) {
			iterator tmp = *this;
			--(*this);
			return tmp;
		}

		value_type *operator->() { return &(nd->value); }

		value_type operator*() { return nd->value; }

	private:
		node_type *nd{};
	};

	struct const_iterator {
		friend class map;
		const_iterator(node_type *_nd) : nd(_nd) {}

		bool operator==(const const_iterator &other) const {
			return other.nd == this->nd;
		}

		bool operator!=(const const_iterator &other) const {
			return !(other == *this);
		}

		const_iterator &operator=(const const_iterator &other) {
			nd = other.nd;
			return *this;
		}

		// ++it
		const_iterator &operator++() {
			rbnode *nxt = rb_next(&(nd->rbn));
			nd          = rb_entry_safe(nxt, node_type, rbn);
			return *this;
		}

		// it++;
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		// --it
		const_iterator &operator--() {
			rbnode *nxt = rb_prev(&(nd->rbn));
			nd          = rb_entry_safe(nxt, node_type, rbn);
			return *this;
		}

		// it--
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			--(*this);
			return tmp;
		}

		const value_type *operator->() { return &(nd->value); }

		value_type operator*() { return nd->value; }

	private:
		node_type *nd{};
	};

	struct reverse_iterator {
		friend class map;
		reverse_iterator(node_type *_nd) : it(_nd) {}

		bool operator==(const reverse_iterator &other) const {
			return it == other.it;
		}

		bool operator!=(const reverse_iterator &other) const {
			return !(other == *this);
		}
		// ++it;
		reverse_iterator &operator++() {
			--it;
			return *this;
		}

		// it++
		reverse_iterator operator++(int) {
			reverse_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		// --it
		reverse_iterator &operator--() {
			++it;
			return *this;
		}

		// it--;
		reverse_iterator operator--(int) {
			reverse_iterator tmp = *this;
			--(*this);
			return tmp;
		}

		value_type *operator->() { return it.operator->(); }

		value_type operator*() { return *it; }

	private:
		iterator it;
	};

	struct const_reverse_iterator {
		friend class map;
		const_reverse_iterator(node_type *_nd) : it(_nd) {}

		bool operator==(const const_reverse_iterator &other) const {
			return it == other.it;
		}

		bool operator!=(const const_reverse_iterator &other) const {
			return !(other == *this);
		}

		// ++it;
		const_reverse_iterator &operator++() {
			--it;
			return *this;
		}

		// it++
		const_reverse_iterator operator++(int) {
			const_reverse_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		// --it
		const_reverse_iterator &operator--() {
			++it;
			return *this;
		}

		// it--;
		const_reverse_iterator operator--(int) {
			const_reverse_iterator tmp = *this;
			--(*this);
			return tmp;
		}

		const value_type *operator->() { return it.operator->(); }

		const value_type operator*() { return *it; }

	private:
		const_iterator it;
	};

	rbroot rbr{};
	size_type _size{0};
	Comp comp{};
};

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::map(std::initializer_list<value_type> init,
                       const Comp &_comp)
    : comp(_comp) {
	for (auto it = init.begin(); it != init.end(); ++it) {
		insert_value(*it);
	}
}

template <typename Key, typename T, typename Comp> map<Key, T, Comp>::~map() {
	clear();
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::mapped_type &map<Key, T, Comp>::at(const key_type &key) {
	node_type *nd = find_node(key);
	return nd->value.second;
}

template <typename Key, typename T, typename Comp>
const map<Key, T, Comp>::mapped_type &
map<Key, T, Comp>::at(const key_type &key) const {
	node_type *nd = find_node(key);
	return nd->value.second;
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::mapped_type &
map<Key, T, Comp>::operator[](const key_type &key) {
	node_type *nd = insert_value({key, mapped_type{}});
	return nd->mapped();
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::mapped_type &map<Key, T, Comp>::operator[](key_type &&key) {
	node_type *nd = insert_value({key, mapped_type{}});
	return nd->mapped();
}

template <typename Key, typename T, typename Comp>
void map<Key, T, Comp>::clear() {
	node_type *nd{nullptr};
	rbnode *rbp = rb_first_postorder(rbr.node);
	while (rbp) {
		nd  = rb_entry(rbp, node_type, rbn);
		rbp = rb_next_postorder(rbp);
		delete nd;
		nd = nullptr;
	}
	_size    = 0;
	rbr.node = nullptr;
}

template <typename Key, typename T, typename Comp>
pair<typename map<Key, T, Comp>::iterator, bool>
map<Key, T, Comp>::insert(const value_type &value) {
	node_type *nd{nullptr};
	node_type *tmp{nullptr};
	rbnode *parent = rbr.node;
	rbnode *cur{nullptr};

	// insert operation
	while (parent) {
		tmp = rb_entry(parent, node_type, rbn);
		if (value.first == tmp->key()) {
			return {iterator(tmp), false};
		}
		if (comp(value.first, tmp->key())) {
			if (parent->left == nullptr) {
				nd           = create_node(value);
				cur          = &(nd->rbn);
				parent->left = cur;
				break;
			}
			parent = parent->left;
		} else {
			if (parent->right == nullptr) {
				nd            = create_node(value);
				cur           = &(nd->rbn);
				parent->right = cur;
				break;
			}
			parent = parent->right;
		}
	}
	if (!parent) {
		nd  = create_node(value);
		cur = &(nd->rbn);
	}
	cur->set_parent_color(parent, RB_RED);
	rb_insert_reblance(cur, &rbr);

	++_size;
	return {iterator(nd), true};
}

template <typename Key, typename T, typename Comp>
pair<typename map<Key, T, Comp>::iterator, bool>
map<Key, T, Comp>::insert(value_type &&value) {
	node_type *nd{nullptr};
	node_type *tmp{nullptr};
	rbnode *parent = rbr.node;
	rbnode *cur{nullptr};

	// insert operation
	while (parent) {
		tmp = rb_entry(parent, node_type, rbn);
		if (value.first == tmp->key()) {
			return {iterator(tmp), false};
		}
		if (comp(value.first, tmp->key())) {
			if (parent->left == nullptr) {
				nd           = create_node(value);
				cur          = &(nd->rbn);
				parent->left = cur;
				break;
			}
			parent = parent->left;
		} else {
			if (parent->right == nullptr) {
				nd            = create_node(value);
				cur           = &(nd->rbn);
				parent->right = cur;
				break;
			}
			parent = parent->right;
		}
	}
	if (!parent) {
		nd  = create_node(value);
		cur = &(nd->rbn);
	}
	cur->set_parent_color(parent, RB_RED);
	rb_insert_reblance(cur, &rbr);

	++_size;
	return {iterator(nd), true};
}

template <typename Key, typename T, typename Comp>
template <typename... Args>
pair<typename map<Key, T, Comp>::iterator, bool>
map<Key, T, Comp>::emplace(Args &&...args) {
	value_type val = make_pair(std::forward<Args>(args)...);
	return insert(val);
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::iterator map<Key, T, Comp>::erase(iterator pos) {
	node_type *nd  = pos.nd;
	node_type *nxt = rb_entry_safe(rb_next(&(nd->rbn)), node_type, rbn);
	iterator ret(nxt);

	rbnode *reblance = rb_erase_node(&(nd->rbn), &rbr);
	if (reblance)
		rb_erase_reblance(reblance, &rbr);

	--_size;
	return ret;
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::size_type
map<Key, T, Comp>::count(const key_type &key) const {
	return find_node(key) != nullptr;
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::iterator map<Key, T, Comp>::find(const key_type &key) {
	node_type *nd = find_node(key);
	return iterator(nd);
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::iterator
map<Key, T, Comp>::lower_bound(const key_type &key) {
	rbnode *rbp = rbr.node;
	node_type *nd{nullptr};
	iterator ret(nullptr);

	while (rbp) {
		nd = rb_entry(rbp, node_type, rbn);
		if (!comp(nd->key(), key)) {
			ret.nd = nd;
			rbp    = rbp->left;
		} else {
			rbp = rbp->right;
		}
	}
	return ret;
}
template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::const_iterator
map<Key, T, Comp>::lower_bound(const key_type &key) const {
	rbnode *rbp = rbr.node;
	node_type *nd{nullptr};
	const_iterator ret(nullptr);

	while (rbp) {
		nd = rb_entry(rbp, node_type, rbn);
		if (!comp(nd->key(), key)) {
			ret.nd = nd;
			rbp    = rbp->left;
		} else {
			rbp = rbp->right;
		}
	}
	return ret;
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::iterator
map<Key, T, Comp>::upper_bound(const key_type &key) {
	rbnode *rbp = rbr.node;
	node_type *nd{nullptr};
	iterator ret(nullptr);

	auto greater = [&](const key_type &lhs, const key_type &rhs) -> bool {
		return !comp(lhs, rhs) && lhs != rhs;
	};

	while (rbp) {
		nd = rb_entry(rbp, node_type, rbn);
		if (greater(nd->key(), key)) {
			ret.nd = nd;
			rbp    = rbp->left;
		} else {
			rbp = rbp->right;
		}
	}
	return ret;
}

template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::const_iterator
map<Key, T, Comp>::upper_bound(const key_type &key) const {

	rbnode *rbp = rbr.node;
	node_type *nd{nullptr};
	const_iterator ret(nullptr);

	auto greater = [&](const key_type &lhs, const key_type &rhs) -> bool {
		return !comp(lhs, rhs) && lhs != rhs;
	};

	while (rbp) {
		nd = rb_entry(rbp, node_type, rbn);
		if (greater(nd->key(), key)) {
			ret.nd = nd;
			rbp    = rbp->left;
		} else {
			rbp = rbp->right;
		}
	}
	return ret;
}

template <typename Key, typename T, typename Comp>
inline map<Key, T, Comp>::node_type *
map<Key, T, Comp>::create_node(const value_type &value) {
	node_type *nd = new node_type(value);
	return nd;
}
template <typename Key, typename T, typename Comp>
inline void map<Key, T, Comp>::destroy_node(node_type *nd) {
	delete nd;
}

template <typename Key, typename T, typename Comp>
inline map<Key, T, Comp>::node_type *
map<Key, T, Comp>::find_node(const key_type &key) const {
	rbnode *rbp = rbr.node;
	node_type *nd{nullptr};

	while (true) {
		nd = rb_entry_safe(rbp, node_type, rbn);
		if (!rbp || key == nd->key()) {
			break;
		} else if (comp(key, nd->key())) {
			rbp = rbp->left;
		} else {
			rbp = rbp->right;
		}
	}
	return nd;
}

template <typename Key, typename T, typename Comp>
void map<Key, T, Comp>::insert_node(node_type *nd) {
	node_type *tmp{nullptr};
	// insert operation
	rbnode *parent = rbr.node;
	rbnode *cur    = &(nd->rbn);

	while (parent) {
		tmp = rb_entry(parent, node_type, rbn);
		if (nd->key() == tmp->key())
			return;
		if (comp(nd->key(), tmp->key())) {
			if (parent->left == nullptr) {
				parent->left = cur;
				break;
			}
			parent = parent->left;
		} else {
			if (parent->right == nullptr) {
				parent->right = cur;
				break;
			}
			parent = parent->right;
		}
	}
	cur->set_parent_color(parent, RB_RED);

	rb_insert_reblance(&(nd->rbn), &rbr);
	++_size;
}
template <typename Key, typename T, typename Comp>
map<Key, T, Comp>::node_type *
map<Key, T, Comp>::insert_value(const value_type &value) {
	node_type *nd{nullptr};
	node_type *tmp{nullptr};
	rbnode *parent = rbr.node;
	rbnode *cur{nullptr};

	// insert operation
	while (parent) {
		tmp = rb_entry(parent, node_type, rbn);
		if (value.first == tmp->key()) {
			return tmp;
		}
		if (comp(value.first, tmp->key())) {
			if (parent->left == nullptr) {
				nd           = create_node(value);
				cur          = &(nd->rbn);
				parent->left = cur;
				break;
			}
			parent = parent->left;
		} else {
			if (parent->right == nullptr) {
				nd            = create_node(value);
				cur           = &(nd->rbn);
				parent->right = cur;
				break;
			}
			parent = parent->right;
		}
	}
	if (!parent) {
		nd  = create_node(value);
		cur = &(nd->rbn);
	}
	cur->set_parent_color(parent, RB_RED);
	rb_insert_reblance(cur, &rbr);

	++_size;
	return nd;
}

} // namespace tp
