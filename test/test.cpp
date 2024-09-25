#include <gtest/gtest.h>

#include "test_map.hpp"
#include "test_rbtree.hpp"
#include "test_unique_ptr.hpp"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
