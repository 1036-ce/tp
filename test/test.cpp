#include <gtest/gtest.h>

#include "test_map.hpp"
#include "test_rbtree.hpp"
#include "test_unique_ptr.hpp"
#include "test_array.hpp"
#include "test_vector.hpp"
#include "test_deque.hpp"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
