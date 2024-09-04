#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>

namespace tp::detail {

template <typename T> struct less {
	bool operator()(const T &lhs, const T &rhs) { return lhs < rhs; }
};

template <typename T> struct skip_list_node {
	T val;
	skip_list_node **forward;
	skip_list_node() : val{}, forward{nullptr} {}
	skip_list_node(T _val, size_t level, skip_list_node *next = nullptr)
	    : val(_val) {
		forward = new skip_list_node *[level + 1];
		for (size_t i = 0; i <= level; ++i)
			forward[i] = next;
	}

	/* size_t level;
	 * skip_list_node() : val{}, level{0}, forward{nullptr} {}
	 * skip_list_node(T _val, size_t _level, skip_list_node* next = nullptr)
	 *     : val(_val), level(_level) {
	 *     forward = new skip_list_node *[level + 1];
	 *     for (size_t i = 0; i <= level; ++i)
	 *         forward[i] = next;
	 * }  */

	~skip_list_node() {
		if (forward)
			delete[] forward;
	}
};

template <typename T, typename Comp = less<T>> class skip_list {
public:
	using node = skip_list_node<T>;

	skip_list() : level(0), length(0), comp(less<T>()) {
		srand(time(0));
		tail = new node();
		head = new node(0, MAXL, tail);
	}

	skip_list(Comp _comp) : level(0), length(0), comp(_comp) {
		srand(time(0));
		tail = new node();
		head = new node(T{}, MAXL, tail);
	}

	~skip_list() {
		while (head != tail) {
			node *tmp = head;
			head      = tmp->forward[0];
			delete tmp;
		}
		delete tail;
	}

	template <typename Func> node *find_if(const T &_val, Func F) {
		node *p = head;
		for (int i = level; i >= 0; --i) {
			while (p->forward[i] != tail && comp(p->forward[i]->val, _val)) {
				p = p->forward[i];
			}
		}
		p = p->forward[0];

		return (F(p->val, _val) && p != tail) ? p : nullptr;
	}

	// time complexity : O(log(n))
	node *find(const T &_val) {
		node *p = head;
		for (int i = level; i >= 0; --i) {
			/* while (p->forward[i] != tail && p->forward[i]->val < _val) {
			 *     p = p->forward[i];
			 * } */
			while (p->forward[i] != tail && comp(p->forward[i]->val, _val)) {
				p = p->forward[i];
			}
		}
		p = p->forward[0];

		return (p->val == _val && p != tail) ? p : nullptr;
	}

	node* insert(const T &_val) & {
		node *update[MAXL + 1];
		node *p = find(_val, update);
		if (p)
			return p;

		size_t lv = random_level();
		if (lv > level) {
			lv         = ++level;
			update[lv] = head;
		}
		node *new_node = new node(_val, lv);
		for (int i = lv; i >= 0; --i) {
			new_node->forward[i]  = update[i]->forward[i];
			update[i]->forward[i] = new_node;
		}
		++length;

		return new_node;
	}

	void remove(const T &_val) & {
		node *update[MAXL + 1];
		node *p = find(_val, update);
		if (!p)
			return;

		for (size_t i = 0; i <= level; ++i) {
			if (update[i]->forward[i] != p)
				break;
			update[i]->forward[i] = p->forward[i];
		}

		delete p;

		while (level > 0 && head->forward[level] == tail)
			--level;

		--length;
	}

	size_t size() const { return length; }

	void debug() {
		for (int i = level; i >= 0; --i) {
			node *p = head->forward[i];
			while (p != tail) {
				std::cout << p->val << ", ";
				p = p->forward[i];
			}
			std::cout << std::endl;
		}
	}

private:
	static constexpr size_t MAXL = 32;
	static constexpr uint32_t P  = 4;
	static constexpr uint32_t S  = 0xFFFF;
	static constexpr uint32_t PS = S / P;

	size_t level, length;
	node *head, *tail;
	Comp comp;

	size_t random_level() {
		size_t lv = 0;
		while ((rand() & S) < PS)
			++lv;
		return lv < MAXL ? lv : MAXL;
	}

	// time complexity : O(log(n))
	node *find(const T &_val, node *update[MAXL + 1]) {
		node *p = head;
		for (int i = level; i >= 0; --i) {
			/* while (p->forward[i] != tail && p->forward[i]->val < _val) {
			 *     p = p->forward[i];
			 * } */
			while (p->forward[i] != tail && comp(p->forward[i]->val, _val)) {
				p = p->forward[i];
			}
			update[i] = p;
		}
		p = p->forward[0];

		return (p->val == _val && p != tail) ? p : nullptr;
	}
};

} // namespace tp::detail
