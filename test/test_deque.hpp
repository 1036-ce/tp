#include <deque.hpp>
// #include <deque>
#include <gtest/gtest.h>

/* class deque_test : public testing::Test {
 * protected:
 *     // virtual void SetUp() will be called before each test is run.  You
 *     // should define it if you need to initialize the varaibles.
 *     // Otherwise, this can be skipped.
 *     virtual void SetUp() {}
 * 
 *     // virtual void TearDown() will be called after each test is run.
 *     // You should define it if there is cleanup work to do.  Otherwise,
 *     // you don't have to provide it.
 *     //
 *     virtual void TearDown() {
 *     }
 * 
 *     deque<int> dq;
 * }; */

// using std::deque;

TEST(deque, ctor_1) {
	deque<int> dq;
	ASSERT_EQ(dq.size(), 0);
}

TEST(deque, ctor_2) {
	deque<int> dq(3);
	ASSERT_EQ(dq.size(), 3);
}

TEST(deque, ctor_3) {
	deque<int> tmp{1,2,3,4,5};
	deque<int> dq1(tmp.begin(), tmp.end());
	deque<int> dq2(tmp.begin() + 1, tmp.begin() + 3);

	ASSERT_EQ(dq1.size(), 5);
	ASSERT_EQ(dq2.size(), 2);
}

TEST(deque, copy_ctor) {
	deque<int> tmp{1,2,3,4};
	deque<int> dq(tmp);

	auto it = dq.begin();
	for (size_t i = 0; i < tmp.size(); ++i, ++it) {
		ASSERT_EQ(*it, tmp.at(i));
	}
}

TEST(deque, move_ctor) {
	deque<int> tmp{1,2,3,4};
	deque<int> dq(std::move(tmp));

	auto it = dq.begin();
	for (size_t i = 0; i < tmp.size(); ++i, ++it) {
		ASSERT_EQ(*it, tmp.at(i));
	}

	ASSERT_EQ(tmp.size(), 0);
}

TEST(deque, initializer_list_ctor) {
	std::initializer_list<int> il{1,2,3,4,5};
	deque<int> dq(il);

	auto it = dq.begin();
	auto tmp = il.begin();
	while (tmp != il.end()) {
		ASSERT_EQ(*tmp, *it);
		++it, ++tmp;
	}
}

// copy operator=
TEST(deque, operator_equal_1) {
	deque<int> dq{1,2,3,4,5};
	deque<int> tmp;
	tmp = dq;
	auto it = tmp.begin();

	for (size_t i = 0;i < dq.size(); ++i, ++it) {
		ASSERT_EQ(*it, dq.at(i));
	}
}

// move operator=
TEST(deque, operator_equal_2) {
	std::initializer_list<int> il{1,2,3,4,5};
	deque<int> dq(il);
	deque<int> tmp;
	tmp = std::move(dq);
	auto it = tmp.begin();
	auto it1 = il.begin();

	ASSERT_EQ(tmp.size(), il.size());
	while (it != tmp.begin()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}

	ASSERT_EQ(dq.size(), 0);
}

TEST(deque, operator_equal_3) {
	std::initializer_list<int> il{1,2,3,4,5};
	deque<int> dq;
	dq = il;

	auto it = dq.begin();
	auto tmp = il.begin();
	while (tmp != il.end()) {
		ASSERT_EQ(*tmp, *it);
		++it, ++tmp;
	}
}

TEST(deque, assign_1) {
	deque<int> dq{1,2,3};

	dq.assign(4, 9);
	auto it = dq.begin();
	while (it != dq.end()) {
		ASSERT_EQ(*it, 9);
		++it;
	}
}

TEST(deque, assign_2) {
	deque<int> dq{1,2,3};
	deque<int> tmp{4,5,6,7,8,9};

	dq.assign(tmp.begin() + 1, tmp.begin() + 3);

	auto lhs = tmp.begin() + 1;
	auto rhs = dq.begin();

	while (rhs != dq.end()) {
		ASSERT_EQ(*lhs, *rhs);
		++lhs, ++rhs;
	}
}

TEST(deque, assign_3) {
	std::initializer_list<int> il{4,5,6,7,7};
	deque<int> dq{1,2,3};

	dq.assign(il);

	auto lhs = il.begin();
	auto rhs = dq.begin();

	while (rhs != dq.end()) {
		ASSERT_EQ(*lhs, *rhs);
		++lhs, ++rhs;
	}
}

TEST(deque, front) {
	deque<int> dq1{1,2,3,4,5};
	const deque<int> dq2{1,2,3,4,5};

	ASSERT_EQ(dq1.front(), 1);
	ASSERT_EQ(dq2.front(), 1);
}

TEST(deque, back) {
	deque<int> dq1{1,2,3,4,5};
	const deque<int> dq2{1,2,3,4,5};

	ASSERT_EQ(dq1.back(), 5);
	ASSERT_EQ(dq2.back(), 5);
}

TEST(deque, iterator) {
	deque<int> dq{1,2,3,4,5};
	auto it = dq.begin();
	ASSERT_EQ(*it, 1);
	++it;
	ASSERT_EQ(*it, 2);
	--it;
	ASSERT_EQ(*it, 1);

	it += 2;
	ASSERT_EQ(*it, 3);
	it -= 2;
	ASSERT_EQ(*it, 1);

	auto tmp = it + 3;
	ASSERT_EQ(*tmp, 4);

	tmp = it++;
	ASSERT_EQ(*tmp, 1);

	tmp = it--;
	ASSERT_EQ(*tmp, 2);

	it = dq.begin();
	auto ed = dq.end();

	ASSERT_EQ(ed - it, dq.size());
}

TEST(deque, const_iterator) {
	deque<int> dq{1,2,3,4,5};
	auto it = dq.cbegin();
	ASSERT_EQ(*it, 1);
	++it;
	ASSERT_EQ(*it, 2);
	--it;
	ASSERT_EQ(*it, 1);

	it += 2;
	ASSERT_EQ(*it, 3);
	it -= 2;
	ASSERT_EQ(*it, 1);

	auto tmp = it + 3;
	ASSERT_EQ(*tmp, 4);

	tmp = it++;
	ASSERT_EQ(*tmp, 1);

	tmp = it--;
	ASSERT_EQ(*tmp, 2);

	it = dq.cbegin();
	auto ed = dq.cend();

	ASSERT_EQ(ed - it, dq.size());
}

TEST(deque, reverse_iterator) {
	deque<int> dq{1,2,3,4,5};
	auto it = dq.rbegin();
	ASSERT_EQ(*it, 5);
	++it;
	ASSERT_EQ(*it, 4);
	--it;
	ASSERT_EQ(*it, 5);

	it += 2;
	ASSERT_EQ(*it, 3);
	it -= 2;
	ASSERT_EQ(*it, 5);

	auto tmp = it + 3;
	ASSERT_EQ(*tmp, 2);

	tmp = it++;
	ASSERT_EQ(*tmp, 5);

	tmp = it--;
	ASSERT_EQ(*tmp, 4);

	it = dq.rbegin();
	auto ed = dq.rend();

	ASSERT_EQ(ed - it, dq.size());
}

TEST(deque, const_reverse_iterator) {
	deque<int> dq{1,2,3,4,5};
	auto it = dq.crbegin();
	ASSERT_EQ(*it, 5);
	++it;
	ASSERT_EQ(*it, 4);
	--it;
	ASSERT_EQ(*it, 5);

	it += 2;
	ASSERT_EQ(*it, 3);
	it -= 2;
	ASSERT_EQ(*it, 5);

	auto tmp = it + 3;
	ASSERT_EQ(*tmp, 2);

	tmp = it++;
	ASSERT_EQ(*tmp, 5);

	tmp = it--;
	ASSERT_EQ(*tmp, 4);

	it = dq.crbegin();
	auto ed = dq.crend();

	ASSERT_EQ(ed - it, dq.size());
}

TEST(deque, clear) {
	deque<int> dq(1000);
	dq.clear();
	ASSERT_EQ(dq.size(), 0);
}

TEST(deque, insert_1) {
	deque<int> dq(20);
	int a = 12;

	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.begin(), a);

	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.3), a);
	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.7), a);

	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.end(), a);

	ASSERT_EQ(dq.size(), 20 + 1024 + 1024 + 1024 + 1024);
}

TEST(deque, insert_2) {
	deque<int> dq(20);

	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.begin(), 12);

	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.3), 12);
	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.7), 12);

	for (size_t i = 0; i < 1024; ++i)
		dq.insert(dq.end(), 12);

	ASSERT_EQ(dq.size(), 20 + 1024 + 1024 + 1024 + 1024);
}

TEST(deque, insert_3) {
	deque<int> dq(20);
	int a = 12;

	for (size_t i = 0; i < 50; ++i)
		dq.insert(dq.begin(), 20, a);

	for (size_t i = 0; i < 50; ++i) {
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.3), 20, a);
	}
	for (size_t i = 0; i < 50; ++i) {
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.7), 20, a);
	}

	for (size_t i = 0; i < 50; ++i)
		dq.insert(dq.end(), 20, a);

	ASSERT_EQ(dq.size(), 20 + 1000 + 1000 + 1000 + 1000);
}

TEST(deque, insert_4) {
	deque<int> dq(20);
	deque<int> tmp(20, 1);

	for (size_t i = 0; i < 50; ++i)
		dq.insert(dq.begin(), tmp.begin(), tmp.end());

	for (size_t i = 0; i < 50; ++i)
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.3), tmp.begin(), tmp.end());

	for (size_t i = 0; i < 50; ++i)
		dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.7), tmp.begin(), tmp.end());

	for (size_t i = 0; i < 50; ++i)
		dq.insert(dq.end(), tmp.begin(), tmp.end());

	ASSERT_EQ(dq.size(), 20 + 1000 + 1000 + 1000 + 1000);
}

TEST(deque, insert_5) {
	deque<int> dq(20);
	std::initializer_list<int> il{1,2,3,4,5};

	dq.insert(dq.begin(), il);

	dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.3), il);

	dq.insert(dq.begin() + static_cast<size_t>(dq.size() * 0.7), il);

	dq.insert(dq.end(), il);

	ASSERT_EQ(dq.size(), 20 + 5 + 5 + 5 + 5);
}

TEST(deque, emplace) {
	deque<int> dq(20);
	int a = 12;

	for (size_t i = 0; i < 1024; ++i)
		dq.emplace(dq.begin(), a);

	for (size_t i = 0; i < 1024; ++i)
		dq.emplace(dq.begin() + static_cast<size_t>(dq.size() * 0.3), a);
	for (size_t i = 0; i < 1024; ++i)
		dq.emplace(dq.begin() + static_cast<size_t>(dq.size() * 0.7), a);

	for (size_t i = 0; i < 1024; ++i)
		dq.emplace(dq.end(), a);

	ASSERT_EQ(dq.size(), 20 + 1024 + 1024 + 1024 + 1024);
}

TEST(deque, erase_1) {
	deque<int> dq(5000);

	for (size_t i = 0; i < 1000; ++i)
		dq.erase(dq.begin());
	for (size_t i = 0; i < 1000; ++i)
		dq.erase(dq.begin() + static_cast<size_t>(dq.size() * 0.3));
	for (size_t i = 0; i < 1000; ++i)
		dq.erase(dq.begin() + static_cast<size_t>(dq.size() * 0.7));
	for (size_t i = 0; i < 1000; ++i)
		dq.erase(dq.end() - 1);

	ASSERT_EQ(dq.size(), 5000 - 1000 - 1000 - 1000 - 1000);
}

TEST(deque, erase_2) {
	deque<int> dq(5000);

	for (size_t i = 0; i < 50; ++i)
		dq.erase(dq.begin(), dq.begin() + 20);

	for (size_t i = 0; i < 50; ++i) {
		auto first = dq.begin() + static_cast<size_t>(dq.size() * 0.3);
		auto last = first + 20;
		dq.erase(first, last);
	}

	for (size_t i = 0; i < 50; ++i) {
		auto first = dq.begin() + static_cast<size_t>(dq.size() * 0.7);
		auto last = first + 20;
		dq.erase(first, last);
	}

	for (size_t i = 0; i < 50; ++i) {
		auto first = dq.end() - 20;
		auto last = dq.end();
		dq.erase(first, last);
	}

	ASSERT_EQ(dq.size(), 1000);
}

TEST(deque, push_back) {
	deque<int> dq;
	int a = 12;

	for (size_t i = 0; i < 1000; ++i)
		dq.push_back(a);

	for (size_t i = 0; i < 1000; ++i)
		dq.push_back(12);

	ASSERT_EQ(dq.size(), 1000 + 1000);
}

TEST(deque, pop_back) {
	deque<int> dq(2000);

	for (size_t i = 0; i < 1000; ++i)
		dq.pop_back();

	ASSERT_EQ(dq.size(), 1000);
}

TEST(deque, emplace_back) {
	deque<int> dq;
	int a = 12;

	for (size_t i = 0; i < 1000; ++i)
		dq.emplace_back(a);

	for (size_t i = 0; i < 1000; ++i)
		dq.emplace_back(12);

	ASSERT_EQ(dq.size(), 1000 + 1000);
}

TEST(deque, push_front) {
	deque<int> dq;
	int a = 12;

	for (size_t i = 0; i < 1000; ++i)
		dq.push_front(a);

	for (size_t i = 0; i < 1000; ++i)
		dq.push_front(12);

	ASSERT_EQ(dq.size(), 1000 + 1000);
}

TEST(deque, pop_front) {
	deque<int> dq(2000);

	for (size_t i = 0; i < 1000; ++i)
		dq.pop_front();

	ASSERT_EQ(dq.size(), 1000);
}

TEST(deque, emplace_front) {
	deque<int> dq;
	int a = 12;

	for (size_t i = 0; i < 1000; ++i)
		dq.emplace_front(a);

	for (size_t i = 0; i < 1000; ++i)
		dq.emplace_front(12);

	ASSERT_EQ(dq.size(), 1000 + 1000);
}

TEST(deuqe, resize_1) {
	deque<int> dq(1000);

	dq.resize(1000);
	ASSERT_EQ(dq.size(), 1000);

	dq.resize(500);
	ASSERT_EQ(dq.size(), 500);

	dq.resize(2000);
	ASSERT_EQ(dq.size(), 2000);
}

TEST(deuqe, resize_2) {
	deque<int> dq(1000, 1);

	dq.resize(1000, 12);
	ASSERT_EQ(dq.size(), 1000);

	auto it = dq.begin();
	while (it != dq.end()) {
		ASSERT_EQ(*it, 1);
		++it;
	}


	dq.resize(500, 13);
	ASSERT_EQ(dq.size(), 500);

	it = dq.begin();
	while (it != dq.end()) {
		ASSERT_EQ(*it, 1);
		++it;
	}

	dq.resize(2000, 14);
	ASSERT_EQ(dq.size(), 2000);

	it = dq.begin();
	while (it != dq.begin() + 500) {
		ASSERT_EQ(*it, 1);
		++it;
	}

	while (it != dq.end()) {
		ASSERT_EQ(*it, 14);
		++it;
	}
}

TEST(deque, swap) {
	deque<int> dq1(1000, 1);
	deque<int> dq2(2000, 2);

	dq1.swap(dq2);

	ASSERT_EQ(dq1.size(), 2000);
	ASSERT_EQ(dq2.size(), 1000);

	auto it = dq1.begin();
	while (it != dq1.end()) {
		ASSERT_EQ(*it, 2);
		++it;
	}

	it = dq2.begin();
	while (it != dq2.end()) {
		ASSERT_EQ(*it, 1);
		++it;
	}
}
