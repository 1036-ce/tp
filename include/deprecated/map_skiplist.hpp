// based on skiplist

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
namespace tp::deprecated {

template <typename T1, typename T2> struct pair {
	using first_type  = T1;
	using second_type = T2;

	pair() = default;
	pair(const first_type &x, const second_type &y) : first(x), second(y) {}

	template <typename U1, typename U2>
	pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}

	pair(const pair &p) = default;
	pair(pair &&p)      = default;

	~pair() = default;

	pair &operator=(const pair &other) {
		this->first  = other.first;
		this->second = other.second;
		return *this;
	}

	void swap(pair &other) {
		pair tmp     = other;
		other.first  = this->first;
		other.second = this->second;
		this->first  = tmp.first;
		this->second = tmp.second;
	}

	first_type first;
	second_type second;
};

template <typename T1, typename T2, typename U1, typename U2>
bool operator==(const pair<T1, T2> &lhs, const pair<U1, U2> &rhs) {
	return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2, typename U1, typename U2>
bool operator<(const pair<T1, T2> &lhs, const pair<U1, U2> &rhs) {
	return lhs.first < rhs.first && lhs.second < rhs.second;
}

template <typename T1, typename T2> pair<T1, T2> make_pair(T1 &&x, T2 &&y) {
	return pair<T1, T2>(x, y);
}

template <typename T> struct less {
	bool operator()(const T &lhs, const T &rhs) const { return lhs < rhs; }
};

template <typename Key, typename T> struct node {
	Key key;
	T val;
	node **forward;
	node() : key{}, val{}, forward{nullptr} {}
	node(Key _key, T _val, size_t level, node *next = nullptr)
	    : key(_key), val(_val) {
		forward = new node *[level + 1];
		for (size_t i = 0; i <= level; ++i)
			forward[i] = next;
	}
	~node() {
		if (forward)
			delete[] forward;
	}
};

template <typename Key, typename T, typename Comp = less<Key>> class map {
public:
	using key_type    = Key;
	using mapped_type = T;
	using value_type  = pair<key_type, mapped_type>;
	using size_type   = size_t;
	using node_type   = node<key_type, mapped_type>;

	struct iterator {
		node_type *node{nullptr};

		iterator() : node(nullptr) {}
		iterator(node_type *_node) : node(_node) {}

		value_type &operator*() { return {node->key, node->val}; }
		node_type *operator->() { return node; }

		// for `++it`
		iterator &operator++() {
			node = node->forward[0];
			return *this;
		}

		iterator operator+(int offset) {
			while (offset-- > 0)
				node = node->forward[0];
			return *this;
		}

		// for `it++`
		iterator operator++(int) {
			iterator tmp = *this;
			node         = node->forward[0];
			return tmp;
		}

		bool operator==(const iterator &other) { return node == other.node; }

		bool operator!=(const iterator &other) { return node != other.node; }
	};

	map() : level(0), length(0), comp(Comp()) {
		srand(time(0));
		tail = new node_type();
		head = new node_type(key_type{}, mapped_type{}, MAXL, tail);
	}

	map(Comp _comp) : level(0), length(0), comp(_comp) {
		srand(time(0));
		tail = new node_type();
		head = new node_type(key_type{}, mapped_type{}, MAXL, tail);
	}

	~map() {
		while (head != tail) {
			node_type *tmp = head;
			head           = tmp->forward[0];
			delete tmp;
		}
		delete tail;
	}

	iterator begin() { return iterator(head->forward[0]); }
	iterator end() { return iterator(tail); }

	bool empty() const { return length == 0; }

	size_type size() const { return length; }

	pair<node_type *, bool> insert(const value_type &_val) {
		node_type *update[MAXL + 1];
		node_type *p = find(_val.first, update);
		if (p)
			return {p, false};

		size_t lv = random_level();
		if (lv > level) {
			lv         = ++level;
			update[lv] = head;
		}
		node_type *new_node = new node_type(_val.first, _val.second, lv);
		for (int i = lv; i >= 0; --i) {
			new_node->forward[i]  = update[i]->forward[i];
			update[i]->forward[i] = new_node;
		}
		++length;
		return {new_node, true};
	}

	template <typename Func> node_type *find_if(const T &_key, Func F) const {
		node_type *p = head;
		for (int i = level; i >= 0; --i) {
			while (p->forward[i] != tail && comp(p->forward[i]->val, _key)) {
				p = p->forward[i];
			}
		}
		p = p->forward[0];

		return (F(p->key, _key) && p != tail) ? p : nullptr;
	}

	size_type count(const T& _key) const {
		if (find(_key))
			return 1;
		return 0;
	}

	bool contaions(const T& _key) const {
		return find(_key) != nullptr;
	}

	// clang-format off
	mapped_type &operator[](const key_type &key) {
		auto node = find(key);
		if (node)
			return node->val;
		else {
			auto [node, _] = insert({key, mapped_type{}});
			return node->val;
		}
	}
	// clang-format on

private:
	static constexpr size_t MAXL = 32;
	static constexpr uint32_t P  = 4;
	static constexpr uint32_t S  = 0xFFFF;
	static constexpr uint32_t PS = S / P;

	size_t level, length;
	node_type *head, *tail;
	Comp comp;

	size_t random_level() {
		size_t lv = 0;
		while ((rand() & S) < PS)
			++lv;
		return lv < MAXL ? lv : MAXL;
	}

	// time complexity : O(log(n))
	node_type *find(const T &_key) const {
		node_type *p = head;
		for (int i = level; i >= 0; --i) {
			while (p->forward[i] != tail && comp(p->forward[i]->key, _key)) {
				p = p->forward[i];
			}
		}
		p = p->forward[0];

		return (p->key == _key && p != tail) ? p : nullptr;
	}

	// time complexity : O(log(n))
	node_type *find(const Key &_key, node_type *update[MAXL + 1]) const {
		node_type *p = head;
		for (int i = level; i >= 0; --i) {
			while (p->forward[i] != tail && comp(p->forward[i]->key, _key)) {
				p = p->forward[i];
			}
			update[i] = p;
		}
		p = p->forward[0];

		return (p->key == _key && p != tail) ? p : nullptr;
	}
};

/* template <typename Key, typename T, typename Comp>
 * struct map<Key, T, Comp>::node {
 *     Key key;
 *     T val;
 *     node **forward;
 *     node() : key{}, val{}, forward{nullptr} {}
 *     node(Key _key, T _val, size_t level, node *next = nullptr)
 *         : key(_key), val(_val) {
 *         forward = new node *[level + 1];
 *         for (size_t i = 0; i <= level; ++i)
 *             forward[i] = next;
 *     }
 *     ~node() {
 *         if (forward)
 *             delete[] forward;
 *     }
 * }; */

} // namespace tp::deprecated
