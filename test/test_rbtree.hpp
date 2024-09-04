#include <gtest/gtest.h>
#include <rbtree.hpp>

#include "utils.hpp"

using namespace tp;

// [node]: black node
// (node): red node


// Case 1: tree is empty
TEST(rbtree, insert_case1) {
	rbtree<int> r;
	r.insert(1);
	auto it = r.begin();

	// ASSERT_TRUE(get_val<int>(it) == 1 && get_color<int>(it) == RB_BLACK);
	ASSERT_TRUE(get_val<int>(it) == 1 && get_color<int>(it) == RB_BLACK);
};

// Case 2: node's parent is black
// insert order: 1, 2
TEST(rbtree, insert_case2) {
	int nnodes        = 2;
	int insert_list[] = {1, 2};

	int inorder_nums[]   = {1, 2};
	int inorder_colors[] = {RB_BLACK, RB_RED};

	rbtree<int> r;

	for (int i = 0; i < nnodes; ++i) {
		r.insert(insert_list[i]);
	}

	auto it = r.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == inorder_nums[i] &&
		            get_color<int>(it) == inorder_colors[i]);
		++it;
	}
};

/*
 * Case 3: node's parent and uncle are both red,
 * then grandparent must be black,
 *
 * insert order: 3, 2, 4, 1 (LL mode)
 * insert order: 3, 1, 4, 2 (LR mode)
 * insert order: 2, 1, 4, 3 (RL mode)
 * insert order: 2, 1, 3, 4 (RR mode)
 *
 */
TEST(rbtree, insert_case3) {
	int nnodes           = 4;
	int insert_list_ll[] = {3, 2, 4,
	                        1}; // parent = gpaent->left && node = parent->left
	int insert_list_lr[] = {3, 1, 4,
	                        2}; // parent = gpaent->left && node = parent->right
	int insert_list_rl[] = {2, 1, 4,
	                        3}; // parent = gpaent->right && node = parent->left
	int insert_list_rr[] = {
	    2, 1, 3, 4}; // parent = gpaent->right && node = parent->right

	int inorder_nums[]      = {1, 2, 3, 4};
	int inorder_colors_ll[] = {RB_RED, RB_BLACK, RB_BLACK, RB_BLACK};
	int inorder_colors_lr[] = {RB_BLACK, RB_RED, RB_BLACK, RB_BLACK};
	int inorder_colors_rl[] = {RB_BLACK, RB_BLACK, RB_RED, RB_BLACK};
	int inorder_colors_rr[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_RED};

	rbtree<int> r_ll, r_lr, r_rl, r_rr;

	for (int i = 0; i < nnodes; ++i) {
		r_ll.insert(insert_list_ll[i]);
		r_lr.insert(insert_list_lr[i]);
		r_rl.insert(insert_list_rl[i]);
		r_rr.insert(insert_list_rr[i]);
	}

	auto it_ll = r_ll.begin();
	auto it_lr = r_lr.begin();
	auto it_rl = r_rl.begin();
	auto it_rr = r_rr.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it_ll) == inorder_nums[i] &&
		            get_color<int>(it_ll) == inorder_colors_ll[i]);
		ASSERT_TRUE(get_val<int>(it_lr) == inorder_nums[i] &&
		            get_color<int>(it_lr) == inorder_colors_lr[i]);
		ASSERT_TRUE(get_val<int>(it_rl) == inorder_nums[i] &&
		            get_color<int>(it_rl) == inorder_colors_rl[i]);
		ASSERT_TRUE(get_val<int>(it_rr) == inorder_nums[i] &&
		            get_color<int>(it_rr) == inorder_colors_rr[i]);
		++it_ll;
		++it_lr;
		++it_rl;
		++it_rr;
	}
};

/*
 * Case 4: node's parent and uncle both are black
 * and parent is grandparent's left child,
 * node is parent's right child (LR mode)
 *
 * or parent is grandparent's right child,
 * node is parent's left child (RL mode)

 *
 *  insert order: 7,5,8,2,1,4,6,3 (LR mode)
 *  insert order: 2,1,7,4,8,3,5,6 (RL mode)
 */
TEST(rbtree, insert_case4) {
	int nnodes           = 8;
	int insert_list_lr[] = {7, 5, 8, 2, 1, 4, 6, 3};
	int insert_list_rl[] = {2, 1, 7, 4, 8, 3, 5, 6};

	int inorder_nums[]      = {1, 2, 3, 4, 5, 6, 7, 8};
	int inorder_colors_lr[] = {RB_BLACK, RB_RED,   RB_RED, RB_BLACK,
	                           RB_BLACK, RB_BLACK, RB_RED, RB_BLACK};
	int inorder_colors_rl[] = {RB_BLACK, RB_RED, RB_BLACK, RB_BLACK,
	                           RB_BLACK, RB_RED, RB_RED,   RB_BLACK};

	rbtree<int> r_lr, r_rl;

	for (int i = 0; i < nnodes; ++i) {
		r_lr.insert(insert_list_lr[i]);
		r_rl.insert(insert_list_rl[i]);
	}

	auto it_lr = r_lr.begin();
	auto it_rl = r_rl.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it_lr) == inorder_nums[i] &&
		            get_color<int>(it_lr) == inorder_colors_lr[i]);
		ASSERT_TRUE(get_val<int>(it_rl) == inorder_nums[i] &&
		            get_color<int>(it_rl) == inorder_colors_rl[i]);
		++it_lr;
		++it_rl;
	}
};

/*
 * Case 5: node's parent and uncle both are black,
 * node is parent's left child and
 * parent is grandparent's left child (LL mode)
 *
 *  insert order: 7,6,8,5,3,2,4,1 (LL mode)
 *  insert order: 2,1,4,3,6,5,7,8 (RR mode)
 */
TEST(rbtree, insert_case5) {
	int nnodes           = 8;
	int insert_list_ll[] = {7, 6, 8, 5, 3, 2, 4, 1};
	int insert_list_rr[] = {2, 1, 4, 3, 6, 5, 7, 8};

	int inorder_nums[]      = {1, 2, 3, 4, 5, 6, 7, 8};
	int inorder_colors_ll[] = {RB_RED,   RB_BLACK, RB_RED, RB_BLACK,
	                           RB_BLACK, RB_BLACK, RB_RED, RB_BLACK};
	int inorder_colors_rr[] = {RB_BLACK, RB_RED, RB_BLACK, RB_BLACK,
	                           RB_BLACK, RB_RED, RB_BLACK, RB_RED};

	rbtree<int> r_ll, r_rr;

	for (int i = 0; i < nnodes; ++i) {
		r_ll.insert(insert_list_ll[i]);
		r_rr.insert(insert_list_rr[i]);
	}

	auto it_ll = r_ll.begin();
	auto it_rr = r_rr.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it_ll) == inorder_nums[i] &&
		            get_color<int>(it_ll) == inorder_colors_ll[i]);
		ASSERT_TRUE(get_val<int>(it_rr) == inorder_nums[i] &&
		            get_color<int>(it_rr) == inorder_colors_rr[i]);
		++it_ll;
		++it_rr;
	}
};

TEST(rbtree, erase_node_have_one_leftchild) {
	int nnodes       = 7;
	int nums[]       = {2, 1, 4, -1, -1, 3, -1};
	color_t colors[] = {
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED, RB_BLACK,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(4);

	int res_nnodes       = 3;
	int res_nums[]       = {1, 2, 3};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

TEST(rbtree, erase_node_have_one_rightchild) {
	int nnodes       = 7;
	int nums[]       = {2, 1, 3, -1, -1, -1, 4};
	color_t colors[] = {
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(3);

	int res_nnodes       = 3;
	int res_nums[]       = {1, 2, 4};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

// successor == node->right
TEST(rbtree, erase_successor_have_rightchild_1) {
	int nnodes       = 7;
	int nums[]       = {2, 1, 3, -1, -1, -1, 4};
	color_t colors[] = {
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(2);

	int res_nnodes       = 3;
	int res_nums[]       = {1, 3, 4};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

// successor != node->right
TEST(rbtree, erase_successor_have_rightchild_2) {
	int nnodes       = 13;
	int nums[]       = {4, 2, 7, 1, 3, 5, 8, -1, -1, -1, -1, -1, 6};
	color_t colors[] = {
	    RB_BLACK, RB_RED,   RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(4);

	int res_nnodes       = 7;
	int res_nums[]       = {1, 2, 3, 5, 6, 7, 8};
	color_t res_colors[] = {
	    RB_BLACK, RB_RED, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED, RB_BLACK,
	};
	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

/**
 * Case 1: P is black, s is red, C is black, D is black
 **/
TEST(rbtree, erase_case1_n_is_leftchild) {

	int nnodes       = 15;
	int nums[]       = {2, 1, 4, -1, -1, 3, 6, -1, -1, -1, -1, -1, -1, 5, -1};
	color_t colors[] = {
	    RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 5;
	int res_nums[]       = {2, 3, 4, 5, 6};
	color_t res_colors[] = {RB_BLACK, RB_RED, RB_BLACK, RB_RED, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

TEST(rbtree, erase_case1_n_is_rightchild) {

	int nnodes       = 9;
	int nums[]       = {5, 3, 6, 1, 4, -1, -1, -1, 2};
	color_t colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_RED};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(6);

	int res_nnodes       = 5;
	int res_nums[]       = {1, 2, 3, 4, 5};
	color_t res_colors[] = {RB_BLACK, RB_RED, RB_BLACK, RB_RED, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

/**
 * Case 2: p is red, S is black, C is black, D is black
 **/
TEST(rbtree, erase_case2) {

	int nnodes       = 7;
	int nums[]       = {3,2,5,-1,-1,4,6};
	color_t colors[] = {RB_RED,RB_BLACK,RB_BLACK,RB_BLACK,RB_BLACK,RB_BLACK,RB_BLACK,};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(2);

	int res_nnodes       = 4;
	int res_nums[]       = {3,4,5,6};
	color_t res_colors[] = {RB_BLACK, RB_BLACK,RB_RED, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

/**
 * Case 3: P is black, S is black, C is black, D is black
 **/
TEST(rbtree, erase_case3) {
	auto init = []() -> rbtree<int> {
		int nnodes       = 7;
		int nums[]       = {4, 2, 6, 1, 3, 5, 7};
		color_t colors[] = {
		    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
		    RB_BLACK, RB_BLACK, RB_BLACK,
		};
		auto r = build_rbtree(nnodes, nums, colors);
		return r;
	};

	int nnodes = 6;

	rbtree<int> r1 = init();
	r1.erase(1);
	int nums1[]       = {2, 3, 4, 5, 6, 7};
	color_t colors1[] = {
	    RB_BLACK, RB_RED, RB_BLACK, RB_BLACK, RB_RED, RB_BLACK,
	};
	auto it1 = r1.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it1) == nums1[i] &&
		            get_color<int>(it1) == colors1[i]);
		++it1;
	}

	rbtree<int> r2 = init();
	r2.erase(3);
	int nums2[]       = {1, 2, 4, 5, 6, 7};
	color_t colors2[] = {RB_RED,   RB_BLACK, RB_BLACK,
	                     RB_BLACK, RB_RED,   RB_BLACK};
	auto it2          = r2.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it2) == nums2[i] &&
		            get_color<int>(it2) == colors2[i]);
		++it2;
	}

	rbtree<int> r3 = init();
	r3.erase(2);
	int nums3[]       = {1, 3, 4, 5, 6, 7};
	color_t colors3[] = {RB_RED,   RB_BLACK, RB_BLACK,
	                     RB_BLACK, RB_RED,   RB_BLACK};
	auto it3          = r3.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it3) == nums3[i] &&
		            get_color<int>(it3) == colors3[i]);
		++it3;
	}

	rbtree<int> r4 = init();
	r4.erase(4);
	int nums4[]       = {1, 2, 3, 5, 6, 7};
	color_t colors4[] = {RB_BLACK, RB_RED,   RB_BLACK,
	                     RB_BLACK, RB_BLACK, RB_RED};
	auto it4          = r4.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it4) == nums4[i] &&
		            get_color<int>(it4) == colors4[i]);
		++it4;
	}

	rbtree<int> r5 = init();
	r5.erase(6);
	int nums5[]       = {1, 2, 3, 4, 5, 7};
	color_t colors5[] = {
	    RB_BLACK, RB_RED, RB_BLACK, RB_BLACK, RB_RED, RB_BLACK,
	};
	auto it5 = r5.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it5) == nums5[i] &&
		            get_color<int>(it5) == colors5[i]);
		++it5;
	}

	rbtree<int> r6 = init();
	r6.erase(5);
	int nums6[]       = {1, 2, 3, 4, 6, 7};
	color_t colors6[] = {RB_BLACK, RB_RED,   RB_BLACK,
	                     RB_BLACK, RB_BLACK, RB_RED};
	auto it6          = r6.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it6) == nums6[i] &&
		            get_color<int>(it6) == colors6[i]);
		++it6;
	}

	rbtree<int> r7 = init();
	r7.erase(7);
	int nums7[]       = {1, 2, 3, 4, 5, 6};
	color_t colors7[] = {RB_BLACK, RB_RED, RB_BLACK,
	                     RB_BLACK, RB_RED, RB_BLACK};
	auto it7          = r7.begin();
	for (int i = 0; i < nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it7) == nums7[i] &&
		            get_color<int>(it7) == colors7[i]);
		++it7;
	}
};

/**
 * Case 4: (p) is red or black, S is black, c is red, D is black
 **/
TEST(rbtree, erase_case4_p_is_black_n_is_leftchild) {
	int nnodes       = 13;
	int nums[]       = {4, 2, 8, 1, 3, 6, 9, -1, -1, -1, -1, 5, 7};
	color_t colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 8;
	int res_nums[]       = {2, 3, 4, 5, 6, 7, 8, 9};
	color_t res_colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

TEST(rbtree, erase_case4_p_is_red_n_is_leftchild) {
	int nnodes       = 21;
	int nums[]       = {10, 4,  12, 2,  8,  11, 13, 1,  3, 6, 9,
	                    -1, -1, -1, -1, -1, -1, -1, -1, 5, 7};
	color_t colors[] = {
	    RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 12;
	int res_nums[]       = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	color_t res_colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	                        RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};
	auto it              = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

TEST(rbtree, erase_case4_p_is_black_n_is_rightchild) {
	int nnodes       = 11;
	int nums[]       = {6, 2, 8, 1, 4, 7, 9, -1, -1, 3, 5};
	color_t colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(9);

	int res_nnodes       = 8;
	int res_nums[]       = {1, 2, 3, 4, 5, 6, 7, 8};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

TEST(rbtree, erase_case4_p_is_red_n_is_rightchild) {
	int nnodes       = 19;
	int nums[]       = {10, 6,  12, 2,  8,  11, 13, 1, 4, 7,
	                    9,  -1, -1, -1, -1, -1, -1, 3, 5};
	color_t colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(9);

	int res_nnodes       = 12;
	int res_nums[]       = {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,
	                        RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};

/**
 * Case 5: (p) is red or black, S is black, (c) is red or black, d is red
 **/
TEST(rbtree, erase_case5_p_is_red_c_is_red_n_is_leftchild) {
	int nnodes       = 11;
	int nums[]       = {6, 2, 7, 1, 4, -1, -1, -1, -1, 3, 5};
	color_t colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_RED,   RB_RED};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 6;
	int res_nums[]       = {2, 3, 4, 5, 6, 7};
	color_t res_colors[] = {RB_BLACK, RB_RED,   RB_RED,
	                        RB_BLACK, RB_BLACK, RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
};
TEST(rbtree, erase_case5_p_is_red_c_is_red_n_is_rightchild) {
	int nnodes       = 9;
	int nums[]       = {6, 4, 7, 2, 5, -1, -1, 1, 3};
	color_t colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_RED,   RB_RED};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(5);

	int res_nnodes       = 6;
	int res_nums[]       = {1, 2, 3, 4, 6, 7};
	color_t res_colors[] = {RB_BLACK, RB_RED,   RB_RED,
	                        RB_BLACK, RB_BLACK, RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

TEST(rbtree, erase_case5_p_is_red_c_is_black_n_is_leftchild) {
	int nnodes       = 23;
	int nums[]       = {10, 4,  12, 2,  6,  11, 13, 1,  3,  5, 8, -1,
	                    -1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 9};
	color_t colors[] = {
	    RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 12;
	int res_nums[]       = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	color_t res_colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	                        RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

TEST(rbtree, erase_case5_p_is_red_c_is_black_n_is_righttchild) {
	int nnodes = 17;
	int nums[] = {10, 6, 12, 4, 8, 11, 13, 2, 5, 7, 9, -1, -1, -1, -1, 1, 3};
	color_t colors[] = {RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                    RB_BLACK, RB_BLACK};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(9);

	int res_nnodes       = 12;
	int res_nums[]       = {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,
	                        RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

TEST(rbtree, erase_case5_p_is_black_c_is_red_n_is_leftchild) {

	int nnodes       = 7;
	int nums[]       = {2, 1, 4, -1, -1, 3, 5};
	color_t colors[] = {
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_RED, RB_RED,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 4;
	int res_nums[]       = {2, 3, 4, 5};
	color_t res_colors[] = {
	    RB_BLACK,
	    RB_RED,
	    RB_BLACK,
	    RB_BLACK,
	};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

TEST(rbtree, erase_case5_p_is_black_c_is_red_n_is_righttchild) {
	int nnodes       = 5;
	int nums[]       = {4, 2, 5, 1, 3};
	color_t colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_RED, RB_RED};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(5);

	int res_nnodes       = 4;
	int res_nums[]       = {1, 2, 3, 4};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_RED, RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}

TEST(rbtree, erase_case5_p_is_black_c_is_black_n_is_leftchild) {
	int nnodes       = 15;
	int nums[]       = {4, 2, 6, 1, 3, 5, 8, -1, -1, -1, -1, -1, -1, 7, 9};
	color_t colors[] = {
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	};
	auto r = build_rbtree(nnodes, nums, colors);

	r.erase(1);

	int res_nnodes       = 8;
	int res_nums[]       = {2, 3, 4, 5, 6, 7, 8, 9};
	color_t res_colors[] = {
	    RB_BLACK, RB_RED,   RB_BLACK, RB_BLACK,
	    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}
TEST(rbtree, erase_case5_p_is_black_c_is_black_n_is_righttchild) {
	int nnodes       = 9;
	int nums[]       = {6, 4, 8, 2, 5, 7, 9, 1, 3};
	color_t colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_RED,  RB_BLACK,
	                    RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK};
	auto r           = build_rbtree(nnodes, nums, colors);

	r.erase(9);

	int res_nnodes       = 8;
	int res_nums[]       = {1, 2, 3, 4, 5, 6, 7, 8};
	color_t res_colors[] = {RB_BLACK, RB_BLACK, RB_BLACK, RB_BLACK,
	                        RB_BLACK, RB_BLACK, RB_RED,   RB_BLACK};

	auto it = r.begin();
	for (int i = 0; i < res_nnodes; ++i) {
		ASSERT_TRUE(get_val<int>(it) == res_nums[i] &&
		            get_color<int>(it) == res_colors[i]);
		++it;
	}
}
