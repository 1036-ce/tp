#include <queue>
#include <rbtree.hpp>

using namespace tp;

template <typename T> T get_val(const typename rbtree<T>::iterator &it) {
	return it.nd->val;
}

template <typename T> color_t get_color(typename rbtree<T>::iterator &it) {
	return it.nd->rbn.color;
}

template <typename T> void set_node_color(node<T> *nd, color_t color) {
	nd->rbn.color = color;
}

// nums[i] == -1 means node is nullptr
inline auto build_rbtree(int nnodes, int nums[], color_t colors[])
    -> rbtree<int> {
	// node<int> *root = new node{.rbn = rbnode(colors[0]), .val = nums[0]};
	node<int> *root = new node(nums[0]);
	set_node_color(root, colors[0]);

	std::queue<node<int> *> que;
	que.push(root);

	for (int i = 1; i < nnodes; i += 2) {
		node<int> *parent = que.front();
		que.pop();
		if (!parent)
			continue;

		rbnode *rbp     = &(parent->rbn);
		node<int> *left = nums[i] == -1 ? nullptr : new node<int>(nums[i]);
		node<int> *right =
		    nums[i + 1] == -1 ? nullptr : new node<int>{nums[i + 1]};

		if (left) {
			left->rbn.parent = rbp;
			set_node_color(left, colors[i]);
		}
		if (right) {
			right->rbn.parent = rbp;
			set_node_color(right, colors[i + 1]);
		}
		rbp->left  = &(left->rbn);
		rbp->right = &(right->rbn);
		que.push(left);
		que.push(right);
	}
	rbtree<int> r(root);
	return r;
}
