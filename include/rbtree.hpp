#pragma once

#include <cassert>
#include <rbtree_impl.hpp>

namespace tp {
template <typename T> struct node {
	rbnode rbn{};
	T val{};

	node() = default;
	node(const T& _val) : val(_val) {}
};

/* 1. 节点为红色或黑色
 * 2. 根节点必须为黑色
 * 3. NIL 节点（空叶子节点）为黑色
 * 4. 红色节点的子节点为黑色
 * 5. 从根节点到 NIL 节点的每条路径上的黑色节点数量相同
 *
 * 1. A node is either red or black
 * 2. The root is black
 * 3. All leaves (NULL) are black
 * 4. Both children of every red node are black
 * 5. Every simple path from root to leaves contains the same number of black
 * nodes. */

template <typename T> class rbtree {
public:
	rbtree() = default;
	rbtree(node<T> *_root) : rbr(&(_root->rbn)) {}

	~rbtree();

	struct iterator;

	iterator begin();
	iterator end();

	node<T> *find(const T &val);

	// insert val to rbtree
	void insert(const T &val);

	// remove val from rbtree
	node<T> *erase(const T &val);

	void clear();

	// for testing
	template <typename U>
	friend bool equal(const rbtree<U> &r1, const rbtree<U> &r2);


private:
	void insert(node<T> *node);
	void erase(node<T> *node);
	rbroot rbr{};
};

template <typename T> struct rbtree<T>::iterator {
	node<T> *nd{nullptr};

	iterator() = default;
	iterator(node<T> *_node) : nd(_node) {}

	bool operator==(const iterator &other) { return other.nd == this->nd; }

	bool operator!=(const iterator &other) { return !(*this == other); }

	T& operator*() { return nd->val; }

	node<T> *operator->() { return nd; }

	iterator &operator++() {
		rbnode *nxt = rb_next(&(nd->rbn));
		nd = rb_entry_safe(nxt, node<T>, rbn);
		return *this;
	}

	iterator operator++(int) {
		iterator tmp = *this;
		++(*this);
		return tmp;
	}

	iterator &operator--() {
		rbnode *pre = rb_prev(&(nd->rbn));
		nd = rb_entry_safe(pre, node<T>, rbn);
		return *this;
	}

	iterator operator--(int) {
		iterator tmp = *this;
		--(*this);
		return tmp;
	}
};

template<typename T> rbtree<T>::~rbtree() {
	clear();
}

template <typename T> inline rbtree<T>::iterator rbtree<T>::begin() {
	rbnode *rbp = rb_first(rbr.node);
	node<T> *nd = rb_entry(rbp, node<T>, rbn);
	return iterator(nd);
}

template <typename T> inline rbtree<T>::iterator rbtree<T>::end() {
	return iterator(nullptr);
}

template <typename T> inline node<T> *rbtree<T>::find(const T &val) {
	rbnode *rbp = rbr.node;
	node<T> *nd{nullptr};

	while (true) {
		nd = rb_entry_safe(rbp, node<T>, rbn);
		if (!rbp || val == nd->val) {
			break;
		} else if (val < nd->val) {
			rbp = rbp->left;
		} else {
			rbp = rbp->right;
		}
	}
	return nd;
}

template <typename T> inline void rbtree<T>::insert(const T &val) {
	node<T> *nd = new node(val);
	insert(nd);
}

/**
 * insert(): insert @node to rbtree
 * @node : node to insert
 */
template <typename T> inline void rbtree<T>::insert(node<T> *nd) {
	// insert operation
	rbnode *parent = rbr.node;
	rbnode *cur    = &(nd->rbn);

	while (parent) {
		if (nd->val < rb_entry(parent, node<T>, rbn)->val) {
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
}

template <typename T> inline node<T> *rbtree<T>::erase(const T &val) {
	node<T> *nd = find(val);
	erase(nd);
	return nd;
}
template <typename T> inline void rbtree<T>::clear() {
	node<T> *nd{nullptr};
	rbnode *rbp = rb_first_postorder(rbr.node);
	while (rbp) {
		nd  = rb_entry(rbp, node<T>, rbn);
		rbp = rb_next_postorder(rbp);
		delete nd;
		nd = nullptr;
	}
	rbr.node = nullptr;
}

template <typename T> inline void rbtree<T>::erase(node<T> *nd) {
	rbnode *reblance = rb_erase_node(&(nd->rbn), &rbr);
	if (reblance)
		rb_erase_reblance(reblance, &rbr);
}

template <typename T> bool equal(const rbtree<T> &r1, const rbtree<T> &r2) {
	auto equal_node = [](auto &&self, node<T> *n1, node<T> *n2) -> bool {
		if (!n1 && !n2)
			return true;

		bool ret = true;
		if (n1 && n2) {
			ret = ret && n1->val == n2->val && n1->color == n2->color;
			ret = ret && self(self, n1->left, n2->left);
			ret = ret && self(self, n1->right, n2->right);

			return ret;
		}
		return false;
	};

	return equal_node(equal_node, r1.root, r2.root);
};
} // namespace tp
