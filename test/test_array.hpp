#include <array.hpp>
#include <gtest/gtest.h>

using namespace tp;

TEST(array, at) {
	array<int, 4> ar{1, 2, 3, 4};
	ASSERT_EQ(ar.at(0), 1);
	ASSERT_EQ(ar.at(1), 2);
	ASSERT_EQ(ar.at(2), 3);
	ASSERT_EQ(ar.at(3), 4);

	const array<int, 3> ar1{1, 2, 3};
	ASSERT_EQ(ar1.at(0), 1);
	ASSERT_EQ(ar1.at(1), 2);
	ASSERT_EQ(ar1.at(2), 3);
}

TEST(array, operator_brackets) {
	array<int, 4> ar{1, 2, 3, 4};
	ASSERT_EQ(ar[0], 1);
	ASSERT_EQ(ar[1], 2);
	ASSERT_EQ(ar[2], 3);
	ASSERT_EQ(ar[3], 4);

	const array<int, 3> ar1{1, 2, 3};
	ASSERT_EQ(ar1[0], 1);
	ASSERT_EQ(ar1[1], 2);
	ASSERT_EQ(ar1[2], 3);
}

TEST(array, front) {
	array<int, 4> ar{1, 2, 3, 4};
	ASSERT_EQ(ar.front(), 1);

	const array<int, 4> ar1{1, 2, 3, 4};
	ASSERT_EQ(ar1.front(), 1);
}

TEST(array, back) {
	array<int, 4> ar{1, 2, 3, 4};
	ASSERT_EQ(ar.back(), 4);

	const array<int, 4> ar1{1, 2, 3, 4};
	ASSERT_EQ(ar1.back(), 4);
}

TEST(array, begin) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.begin();
	ASSERT_EQ(*it, 1);

	const array<int, 4> ar1{1, 2, 3, 4};
	auto it1 = ar1.begin();
	ASSERT_EQ(*it1, 1);
}

TEST(array, cbegin) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.cbegin();
	ASSERT_EQ(*it, 1);
}

TEST(array, end) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.end();
	ASSERT_EQ(*(it - 1), 4);

	const array<int, 4> ar1{1, 2, 3, 4};
	auto it1 = ar1.end();
	ASSERT_EQ(*(it1 - 1), 4);
}

TEST(array, cend) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.cend();
	ASSERT_EQ(*(it - 1), 4);
}

TEST(array, rbegin) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.rbegin();
	ASSERT_EQ(*it, 4);

	const array<int, 4> ar1{1, 2, 3, 4};
	auto it1 = ar1.rbegin();
	ASSERT_EQ(*it1, 4);
}

TEST(array, crbegin) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.crbegin();
	ASSERT_EQ(*it, 4);
}

TEST(array, rend) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.rend();
	ASSERT_EQ(*(it - 1), 1);

	const array<int, 4> ar1{1, 2, 3, 4};
	auto it1 = ar1.rend();
	ASSERT_EQ(*(it1 - 1), 1);
}

TEST(array, crend) {
	array<int, 4> ar{1, 2, 3, 4};
	auto it = ar.crend();
	ASSERT_EQ(*(it - 1), 1);
}

TEST(array, empty) {
	array<int, 4> ar{1, 2, 3, 4};
	ASSERT_EQ(ar.empty(), false);
	array<int, 0> ar1;
	ASSERT_EQ(ar1.empty(), true);
}

TEST(array, size) {
	array<int, 4> ar{1, 2, 3, 4};
	ASSERT_EQ(ar.size(), 4);
	array<int, 0> ar1;
	ASSERT_EQ(ar1.size(), 0);
}

TEST(array, fill) {
	array<int, 4> ar;
	ar.fill(2);
	for (std::size_t i = 0; i < ar.size(); ++i)
		ASSERT_EQ(ar.at(i), 2);
}
