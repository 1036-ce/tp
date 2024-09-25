#include <unique_ptr.hpp>
#include <gtest/gtest.h>

TEST(unique_ptr, ctor_1) {
	unique_ptr<int> u1{};
	ASSERT_EQ(u1.get(), nullptr);
}

TEST(unique_ptr, ctor_2) {
	int *ptr = new int(1);

	unique_ptr<int> u1{ptr};
	ASSERT_EQ(u1.get(), ptr);
}

TEST(unique_ptr, ctor_3) {
	int *ptr = new int(1);
	unique_ptr<int> u1(ptr);
	unique_ptr<int> u2(std::move(u1));
	ASSERT_EQ(u1.get(), nullptr);
	ASSERT_EQ(u2.get(), ptr);
}

TEST(unique_ptr, operator_eq_1) {
	int *ptr = new int(1);
	unique_ptr<int> u1(ptr);
	unique_ptr<int> u2{};

	u2 = std::move(u1);

	ASSERT_EQ(u1.get(), nullptr);
	ASSERT_EQ(u2.get(), ptr);
}

TEST(unique_ptr, operator_eq_2) {
	unique_ptr<int> u1;
	u1 = nullptr;

	ASSERT_EQ(u1.get(), nullptr);
}

TEST(unique_ptr, release) {
	int *ptr = new int(1);
	unique_ptr<int> u1(ptr);
	auto p = u1.release();
	ASSERT_EQ(ptr, p);
}

TEST(unique_ptr, reset) {
	unique_ptr<int> u1(new int(1));

	u1.reset();
	ASSERT_EQ(u1.get(), nullptr);

	unique_ptr<int> u2(new int(1));

	u2.reset(new int(2));

	ASSERT_EQ(*u2, 2);
}
