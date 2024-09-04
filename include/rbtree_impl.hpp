#pragma once

#include <cstddef>  // for: offsetof

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

enum color_t { RB_RED, RB_BLACK };

struct rbnode {
	rbnode *parent{nullptr};
	rbnode *left{nullptr};
	rbnode *right{nullptr};
	color_t color{RB_RED};

	rbnode() = default;
	rbnode(const color_t& _color) : color(_color) {}

	bool is_red() { return color == RB_RED; }

	bool is_black() { return color == RB_BLACK; }

	void set_parent_color(rbnode *_parent, color_t _color) {
		parent = _parent;
		color  = _color;
	}

	rbnode *successor() {
		if (!right)
			return nullptr;
		rbnode *ret = right;
		while (ret->left)
			ret = ret->left;
		return ret;
	}
};

struct rbroot {
	rbnode *node{nullptr};
};

#define container_of(ptr, type, member) \
	((type*)((char*)(ptr) - offsetof(type, member)))

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define rb_entry_safe(ptr, type, member) \
	(ptr == nullptr ? nullptr : container_of(ptr, type, member))


// functions' declaration

static inline void rb_insert_reblance(rbnode *node, rbroot *root);

static inline void rb_erase(rbnode *node, rbroot *root);

static inline rbnode* rb_erase_node(rbnode *node, rbroot *root);

static inline void rb_erase_reblance(rbnode *parent, rbroot *root);

static inline rbnode* rb_first(rbnode *root);

static inline rbnode* rb_last(rbnode *root);

static inline rbnode* rb_next(rbnode *node);

static inline rbnode* rb_prev(rbnode *node);

static inline rbnode* rb_first_postorder(rbnode *root);

static inline rbnode* rb_last_postorder(rbnode *root);

static inline rbnode* rb_next_postorder(rbnode *root);

static inline rbnode* rb_prev_postorder(rbnode *root);

static inline rbnode* rb_left_deepest_node(rbnode *node);

static inline void rb_change_child(rbnode *cur, rbnode *nxt, rbnode *parent,
		rbroot* root);


// functions' definetion

static inline void rb_insert_reblance(rbnode *node, rbroot *root) {
	rbnode *parent = node->parent, *gparent, *tmp;

	while (true) {
		/*
		 * Loop invariant: node is red
		 */

		// Case 1: tree is empty
		if (!parent) {
			node->set_parent_color(nullptr, RB_BLACK);
			root->node = node;
			break;
		}

		// Case 2: node's parent is black
		if (parent->is_black())
			break;

		// because parent is red and rule 2), grandparent must not be NULL.
		gparent = parent->parent;
		tmp     = gparent->right;

		if (parent != tmp) { // parent == gparent->left, tmp is uncle node
			if (tmp && tmp->is_red()) {
				/*
				 * Case 3: node's parent and uncle are both red,
				 * then grandparent must be black,
				 * make parent and uncle black, grandparent red
				 *
				 *		  G             g
				 *		 / \           / \
				 *		p   u  -->    P   U
				 *		 \             \
				 *		  n             n
				 * However, because of grandparent's parent might be red,
				 * and 4) do not allow this,
				 * so we need to recurse at grandparent node
				 */

				parent->color  = RB_BLACK;
				tmp->color     = RB_BLACK;
				gparent->color = RB_RED;

				node   = gparent;
				parent = gparent->parent;
				continue;
			}

			tmp = parent->right;
			if (node == tmp) { // node == parent->right;
				/*
				 * Case 4: node's parent and uncle both are black
				 * and parent is grandparent's left child,
				 * node is parent's right child (LR mode)
				 *
				 * we need to left rotate at parent, then rotate at G
				 *
				 *      G             G   
				 *     / \           / \  
				 *    p   U  -->    n   U 
				 *     \           /      
				 *      n         p       
				 *
				 * However, this is still violates of rule 4),
				 * we will fix it at Case 5
				 */
				tmp           = node->left;
				parent->right = tmp;
				if (tmp)
					tmp->parent = parent;

				node->left     = parent;
				node->parent   = gparent;
				parent->parent = node;
				rb_change_child(parent, node, gparent, root);

				parent = node;
				tmp    = parent->right;
			}

			/*
			 * Case 5: node's parent and uncle both are black,
			 * node is parent's left child and
			 * parent is grandparent's left child (LL mode)
			 *
			 * we need to rotate right at grandparent
			 *          G             P
			 *         / \           / \
			 *        p   U  -->    n   g
			 *       /                  \
			 *      n                    U
			 */
			gparent->left = tmp;
			if (tmp)
				tmp->parent = gparent;

			parent->right = gparent;
			// parent->parent = gparent->parent;
			rb_change_child(gparent, parent, gparent->parent, root);

			parent->set_parent_color(gparent->parent, RB_BLACK);
			gparent->set_parent_color(parent, RB_RED);

			/* parent->color  = RB_BLACK;
			 * gparent->color = RB_RED; */

			break;
		} else { // parent == gparent->right
			tmp = gparent->left;
			if (tmp && tmp->is_red()) {
				/*
				 * Case 3: node's parent and uncle are both red,
				 * then grandparent must be black,
				 * make parent and uncle black, grandparent red
				 *
				 *		  G             g
				 *		 / \           / \
				 *		u   p  -->    U   P
				 *		   /             /
				 *		  n             n
				 * However, because of grandparent's parent might be red,
				 * and 4) do not allow this,
				 * so we need to recurse at grandparent node
				 */

				parent->color  = RB_BLACK;
				tmp->color     = RB_BLACK;
				gparent->color = RB_RED;

				node   = gparent;
				parent = gparent->parent;
				continue;
			}

			tmp = parent->left;
			if (node == tmp) { // node == parent->left;
				/*
				 * Case 4: node's parent and uncle both are black
				 * and parent is grandparent's right child,
				 * node is parent's left child (RL mode)
				 *
				 * we need to right rotate at parent
				 *
				 *      G             G
				 *     / \           / \
				 *    U   p  -->    U   n
				 *       /               \
				 *      n                 p
				 *
				 * However, this is still violates of rule 4),
				 * we will fix it at Case 5
				 */
				tmp          = node->right;
				parent->left = tmp;
				if (tmp)
					tmp->parent = parent;

				node->right    = parent;
				node->parent   = gparent;
				parent->parent = node;
				rb_change_child(parent, node, gparent, root);

				parent = node;
				tmp    = parent->left;
			}

			/*
			 * Case 5: node's parent and uncle both are black,
			 * node is parent's right child and
			 * parent is grandparent's right child (RR mode)
			 *
			 * we need to rotate right at grandparent
			 *          G             P
			 *         / \           / \
			 *        U   p  -->    g   n
			 *             \       /
			 *              n     U
			 */
			gparent->right = tmp;
			if (tmp)
				tmp->parent = gparent;

			parent->left = gparent;
			// parent->parent = gparent->parent;
			rb_change_child(gparent, parent, gparent->parent, root);
			parent->set_parent_color(gparent->parent, RB_BLACK);
			gparent->set_parent_color(parent, RB_RED);

			/* parent->color  = RB_BLACK;
			 * gparent->color = RB_RED; */

			break;
		}
	}
}


static inline void rb_erase(rbnode *node, rbroot *root) {
	rbnode *reblance = rb_erase_node(node, root);
	if (reblance)
		rb_erase_reblance(reblance, root);
}

static inline rbnode* rb_erase_node(rbnode *node, rbroot *root) {
	rbnode *reblance = nullptr, *child = nullptr;
	rbnode *parent = node->parent;

	// case 1: node doesn't have left child and right child
	if (!node->left && !node->right) {
		if (node->color == RB_BLACK)
			reblance = node->parent;
		rb_change_child(node, static_cast<rbnode*>(nullptr), parent, root);
	}
	// case 2: node only have one child (here is left child)
	else if (node->left && !node->right) {
		child         = node->left;
		child->parent = node->parent;
		rb_change_child(node, child, parent, root);
		child->color = RB_BLACK;
	}
	// case 2: node only have one child (here is right child)
	else if (!node->left && node->right) {
		child         = node->right;
		child->parent = node->parent;
		rb_change_child(node, child, parent, root);
		child->color = RB_BLACK;
	}
	// case 3: node have both left child and right child
	else {
		rbnode *successor = node->successor();

		// successor is node's right child
		if (successor == node->right) {
			successor->left         = node->left;
			successor->left->parent = successor;

			successor->parent = node->parent;
			rb_change_child(node, successor, parent, root);
			color_t tmp      = successor->color;
			successor->color = node->color;

			parent = successor;

			// successor has right child
			if (successor->right) {
				successor->right->color = RB_BLACK;
			}
			// successor doesn't have right child
			else {
				reblance = (tmp == RB_RED) ? nullptr : parent;
			}
		} else {
			parent       = successor->parent;
			child        = successor->right;
			parent->left = child;

			successor->parent = node->parent;
			rb_change_child(node, successor, node->parent, root);
			color_t tmp      = successor->color;
			successor->color = node->color;

			successor->left          = node->left;
			successor->left->parent  = successor;
			successor->right         = node->right;
			successor->right->parent = successor;

			// successor has right child
			if (child) {
				child->parent = parent;
				child->color  = RB_BLACK;
			}
			// successor doesn't have right child
			else {
				// reblance = node->is_red() ? nullptr : parent;
				reblance = (tmp == RB_RED) ? nullptr : parent;
			}
		}
	}

	node->parent = nullptr;
	node->left = nullptr;
	node->right = nullptr;
	node->color = RB_RED;

	return reblance;
}

static inline void rb_erase_reblance(rbnode *parent, rbroot *root) {
	rbnode *node = nullptr, *slibing, *tmp1, *tmp2;

	while (true) {
		/*
		 * Loop invariants:
		 * - node is black (or NULL on first iteration)
		 * - node is not the root (parent is not NULL)
		 * - All leaf paths going through parent and node have a
		 *   black node count that is 1 lower than other leaf paths.
		 */
		slibing = parent->right;
		if (node != slibing) { // node == parent->left
			if (slibing->is_red()) {
				/**
				 * Case 1: P is black, s is red, C is black, D is black
				 *
				 * 1. left rotate at P
				 * 2. make P red, s black
				 * 3. recurse at p
				 * 3. However, because of N subtree's black depth is 1 lower 
				 * than C subtree, and 5) do not allow this, 
				 * so we need to recurse at parent node;
				 *  	P           s          S
				 *     / \         / \        / \
				 *    N   s  -->  P   D -->  p   D
				 *  	 / \     / \        / \
				 *  	C   D   N   C      N   C
				 **/
				tmp1 = slibing->left; // tmp1 == C

				parent->right = tmp1;
				tmp1->parent  = parent;

				rb_change_child(parent, slibing, parent->parent, root);
				slibing->parent = parent->parent;

				slibing->left  = parent;
				parent->parent = slibing;

				slibing->color = RB_BLACK;
				parent->color  = RB_RED;

				slibing = tmp1;
			}

			tmp1 = slibing->right;               // D is tmp1
			if (!tmp1 || tmp1->is_black()) {     // D is black
				tmp2 = slibing->left;            // C is tmp2
				if (!tmp2 || tmp2->is_black()) { // C is black
					if (parent->is_red()) {      // p is red
						/**
						 * Case 2: p is red, S is black, C is black, D is black
						 * then make p black, S red
						 * 		 p           P
						 *      / \         / \
						 *     N   S  -->  N   s
						 *    	  / \         / \
						 *   	 C   D       C   D
						 **/
						parent->color  = RB_BLACK;
						slibing->color = RB_RED;
					} else {
						/**
						 * Case 3: P is black, S is black, C is black, D is black
						 * then make S red
						 *
						 * However, because of P subtree's black depth is 1 lower 
						 * than P's slibing subtree, and 5) do not allow this, 
						 * so we need to recurse at P's parent node;
						 *       P           P
						 *      / \         / \
						 *     N   S  -->  N   s
						 *    	  / \         / \
						 *   	 C   D       C   D
						 **/
						slibing->color = RB_RED;
						node           = parent;
						parent         = parent->parent;
						if (parent)
							continue;
					}
					break;
				}
				// c is red

				/**
				 * Case 4: (p) is red or black, S is black, c is red, D is black
				 * right rotate at S, make S red, c black
				 * then turn to Case 5
				 *  (p)         (p)              (p)
				 *  / \         / \              / \
				 * N   S  -->  N   c      -->   N   C
				 *    / \           \                \
				 * 	 c   D           S                s
				 *	 				  \                \
				 *					   D                D
				 **/
				slibing->left = tmp2->right;
				if (tmp2->right)
					tmp2->right->parent = slibing;

				slibing->parent = tmp2;
				tmp2->right     = slibing;

				tmp2->parent  = parent;
				parent->right = tmp2;

				tmp2->color    = RB_BLACK;
				slibing->color = RB_RED;

				slibing = tmp2;
				tmp1    = slibing->right;
				tmp2    = slibing->left;
				/* tmp1 = slibing->left;
				 * tmp2 = slibing->right; */
			}
			// D is red
			/**
			 * Case 5: (p) is red or black, S is black, (c) is red or black, d
			 *is red
			 *
			 * 1. left rotate at (p)
			 * 2. swap (p)'s color and S's color
			 * 3. make d black
			 *
			 *  (p)           S           (s)
			 *  / \          / \          / \
			 * N   S  -->  (p)  d  -->   P   D
			 *    / \      / \          / \
			 * 	(c)  d    N  (c)       N  (c)
			 **/
			tmp2 = slibing->left; // C is tmp2

			rb_change_child(parent, slibing, parent->parent, root);
			slibing->parent = parent->parent;

			slibing->left  = parent;
			parent->parent = slibing;

			parent->right = tmp2;
			if (tmp2)
				tmp2->parent = parent;

			slibing->color = parent->color;
			parent->color  = RB_BLACK;
			tmp1->color    = RB_BLACK;
			break;
		} else { // node == parent->right
			slibing = parent->left;
			if (slibing->is_red()) {
				/**
				 * Case 1: P is black, s is red, C is black, D is black
				 **/
				tmp1 = slibing->right; // tmp1 == C

				parent->left = tmp1;
				tmp1->parent = parent;

				rb_change_child(parent, slibing, parent->parent, root);
				slibing->parent = parent->parent;

				slibing->right = parent;
				parent->parent = slibing;

				slibing->color = RB_BLACK;
				parent->color  = RB_RED;

				slibing = tmp1;
			}

			tmp1 = slibing->left;                // D is tmp1
			if (!tmp1 || tmp1->is_black()) {     // D is black
				tmp2 = slibing->right;           // C is tmp2
				if (!tmp2 || tmp2->is_black()) { // C is black
					if (parent->is_red()) {      // p is red
						/**
						 * Case 2: p is red, S is black, C is black, D is black
						 * then make p black, S red
						 **/
						parent->color  = RB_BLACK;
						slibing->color = RB_RED;
					} else {
						/**
						 * Case 3: P is black, S is black, C is black, D is
						 *black
						 **/
						slibing->color = RB_RED;
						node           = parent;
						parent         = parent->parent;
						if (parent)
							continue;
					}
					break;
				}
				// c is red

				/**
				 * Case 4: (p) is red or black, S is black, c is red, D is black
				 * left rotate at S, make S red, c black
				 * then turn to Case 5
				 **/
				slibing->right = tmp2->left;
				if (tmp2->left)
					tmp2->left->parent = slibing;

				slibing->parent = tmp2;
				tmp2->left      = slibing;

				tmp2->parent = parent;
				parent->left = tmp2;

				tmp2->color    = RB_BLACK;
				slibing->color = RB_RED;

				slibing = tmp2;
				tmp1    = slibing->left;
				tmp2    = slibing->right;
			}
			// D is red
			/**
			 * Case 5: (p) is red or black, S is black, (c) is red or black, d
			 *is red
			 *
			 * 1. right rotate at (p)
			 * 2. swap (p)'s color and S's color
			 * 3. make d black
			 **/
			tmp2 = slibing->right; // C is tmp2
			rb_change_child(parent, slibing, parent->parent, root);
			slibing->parent = parent->parent;

			slibing->right = parent;
			parent->parent = slibing;

			parent->left = tmp2;
			if (tmp2)
				tmp2->parent = parent;

			slibing->color = parent->color;
			parent->color  = RB_BLACK;
			tmp1->color    = RB_BLACK;
			break;
		}
	}
}

static inline rbnode* rb_first(rbnode *root) {
	rbnode *rbp = root;
	if (!rbp)
		return nullptr;

	while (rbp->left)
		rbp = rbp->left;
	return rbp;
}

static inline rbnode* rb_last(rbnode *root) {
	rbnode *rbp = root;
	if (!rbp)
		return nullptr;

	while (rbp->right)
		rbp = rbp->right;
	return rbp;
}

static inline rbnode* rb_next(rbnode *node) {
	if (!node)
		return nullptr;

	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;
		return node;
	}

	rbnode *parent{nullptr};
	while ((parent = node->parent) && node == parent->right)
		node = parent;
	return parent;
}

static inline rbnode* rb_prev(rbnode *node) {
	if (!node)
		return nullptr;

	if (node->left) {
		node = node->left;
		while (node->right)
			node = node->right;
		return node;
	}

	rbnode *parent;
	while ((parent = node->parent) && node == parent->left)
		node = parent;
	return parent;
}

static inline rbnode* rb_first_postorder(rbnode *root) {
	if (!root)
		return nullptr;

	return rb_left_deepest_node(root);
}

static inline rbnode* rb_last_postorder(rbnode *root) {
	return root;
}

static inline rbnode* rb_next_postorder(rbnode *node) {
	if (!node)
		return nullptr;
	rbnode *parent = node->parent;
	if (parent && node == parent->left && parent->right)
		return rb_left_deepest_node(parent->right);
	else
		return parent;
}

static inline rbnode* rb_prev_postorder(rbnode *node) {
	if (!node)
		return nullptr;

	if (node->right)
		return node->right;
	return node->left;
}

static inline rbnode* rb_left_deepest_node(rbnode *node) {
	while (true) {
		if (node->left)
			node = node->left;
		else if (node->right)
			node = node->right;
		else
			return node;
	}
}

static inline void rb_change_child(rbnode *cur, rbnode *nxt, rbnode *parent,
		rbroot* root) {
	if (parent) {
		if (cur == parent->left)
			parent->left = nxt;
		else
			parent->right = nxt;
	} else
		root->node = nxt;
}
