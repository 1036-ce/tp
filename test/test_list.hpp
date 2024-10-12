#include <list.hpp>
#include <gtest/gtest.h>

inline auto inc_iter(list<int>::iterator it, std::size_t num)-> decltype(it) {
	while (num--)
		++it;
	return it;
}

inline auto dec_iter(list<int>::iterator it, std::size_t num) -> decltype(it) {
	while (num--)
		--it;
	return it;
}

TEST(list, ctor_1) {
	std::initializer_list<int> il{1,2,3,4,5};
	list<int> lt(il);

	auto it1 = lt.begin();
	auto it2 = il.begin();

	while (it2 != il.end()) {
		ASSERT_EQ(*it1, *it2);
		++it1, ++it2;
	}
	ASSERT_EQ(lt.size(), il.size());
	ASSERT_EQ(lt.empty(), false);
}

TEST(list, emplace) {
	std::initializer_list<int> il1{1,2,3,4,5};
	std::initializer_list<int> target{9,1,9,2,9,3,4,5,9};
	list<int> lt(il1);

	auto it = lt.begin();
	lt.emplace(it, 9);

	it = inc_iter(lt.begin(), 2);
	lt.emplace(it, 9);

	it = inc_iter(lt.begin(), 4);
	lt.emplace(it, 9);

	it = lt.end();
	lt.emplace(it, 9);

	auto it1 = lt.begin();
	auto it2 = target.begin();

	while (it2 != target.end()) {
		ASSERT_EQ(*it1, *it2);
		++it1, ++it2;
	}
	ASSERT_EQ(lt.size(), target.size());
	ASSERT_EQ(lt.empty(), false);
}

TEST(list, insert_1) {
	std::initializer_list<int> il1{1,2,3,4,5};
	std::initializer_list<int> target{9,1,9,2,9,3,4,5,9};
	list<int> lt(il1);
	int a = 9;

	auto it = lt.begin();
	lt.insert(it, a);

	it = inc_iter(lt.begin(), 2);
	lt.insert(it, a);

	it = inc_iter(lt.begin(), 4);
	lt.insert(it, a);

	it = lt.end();
	lt.insert(it, a);

	auto it1 = lt.begin();
	auto it2 = target.begin();

	while (it2 != target.end()) {
		ASSERT_EQ(*it1, *it2);
		++it1, ++it2;
	}
	ASSERT_EQ(lt.size(), target.size());
	ASSERT_EQ(lt.empty(), false);
}

TEST(list, insert_2) {
	std::initializer_list<int> il1{1,2,3,4,5};
	std::initializer_list<int> target{9,1,9,2,9,3,4,5,9};
	list<int> lt(il1);

	auto it = lt.begin();
	lt.insert(it, 9);

	it = inc_iter(lt.begin(), 2);
	lt.insert(it, 9);

	it = inc_iter(lt.begin(), 4);
	lt.insert(it, 9);

	it = lt.end();
	lt.insert(it, 9);

	auto it1 = lt.begin();
	auto it2 = target.begin();

	while (it2 != target.end()) {
		ASSERT_EQ(*it1, *it2);
		++it1, ++it2;
	}
	ASSERT_EQ(lt.size(), target.size());
	ASSERT_EQ(lt.empty(), false);
}

TEST(list, insert_3) {
	std::initializer_list<int> il1{1,2,3,4,5};
	std::initializer_list<int> target{1,9,9,9,9,2,3,4,5};
	list<int> lt(il1);
	list<int> lt2{9,9,9,9};

	lt.insert(inc_iter(lt.begin(), 1), lt2.begin(), lt2.end());

	auto it1 = lt.begin();
	auto it2 = target.begin();

	while (it2 != target.end()) {
		ASSERT_EQ(*it1, *it2);
		++it1, ++it2;
	}
	ASSERT_EQ(lt.size(), target.size());
	ASSERT_EQ(lt.empty(), false);
}

TEST(list, erase_1) {
	list<int> lt{1, 2, 3, 4};
	std::initializer_list<int> target{1,2,4};

	auto tt = inc_iter(lt.begin(), 2);
	auto tmp = lt.erase(tt);
	ASSERT_EQ(*tmp, 4);

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}

TEST(list, erase_2) {
	list<int> lt{1, 2, 3, 4};
	std::initializer_list<int> target{3,4};

	auto tt = inc_iter(lt.begin(), 2);
	auto tmp = lt.erase(lt.begin(), tt);
	ASSERT_EQ(*tmp, 3);

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}

TEST(list, pop_back) {
	list<int> lt{1, 2, 3, 4};
	std::initializer_list<int> target{1,2};

	lt.pop_back();
	lt.pop_back();

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}

TEST(list, push_front_1) {
	list<int> lt{1,2,3,4};
	std::initializer_list<int> target{9,9,9,1,2,3,4};
	int a = 9;

	lt.push_front(a);
	lt.push_front(a);
	lt.push_front(a);

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}

TEST(list, push_front_2) {
	list<int> lt{1,2,3,4};
	std::initializer_list<int> target{9,9,9,1,2,3,4};

	lt.push_front(9);
	lt.push_front(9);
	lt.push_front(9);

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}

TEST(list, emplace_front) {
	list<int> lt{1,2,3,4};
	std::initializer_list<int> target{9,9,9,1,2,3,4};

	lt.emplace_front(9);
	lt.emplace_front(9);
	lt.emplace_front(9);

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}

TEST(list, pop_front) {
	list<int> lt{1,2,3,4};
	std::initializer_list<int> target{3,4};

	lt.pop_front();
	lt.pop_front();

	auto it = lt.begin();
	auto it1 = target.begin();
	while (it != lt.end()) {
		ASSERT_EQ(*it, *it1);
		++it, ++it1;
	}
}
