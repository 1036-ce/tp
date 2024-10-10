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
