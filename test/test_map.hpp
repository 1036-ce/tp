#include <map.hpp>
#include <gtest/gtest.h>

using namespace tp;

TEST(map, at) {
	map<int, int> mp1{
		{1,1},
		{2,2}
	};
	ASSERT_EQ(mp1.at(1), 1);

	const map<int, int> mp2{
		{1,1},
		{2,2}
	};

	ASSERT_EQ(mp2.at(1), 1);
}; 

TEST(map, operator_square_brackets) {
	map<int, int> mp{
		{1,1},
		{2,2}
	};
	mp[3] = 3;

	ASSERT_EQ(mp[1], 1);
	ASSERT_EQ(mp[3], 3);
};

TEST(map, iterator) {
	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};

	int key = 1, val = 1;
	auto it = mp.begin();
	while (it != mp.end()) {
		ASSERT_EQ(it->first, key);
		ASSERT_EQ(it->second, val);
		++key, ++val;
		++it;
	}
};

TEST(map, const_iterator) {
	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};

	int key = 1, val = 1;
	auto it = mp.cbegin();
	while (it != mp.cend()) {
		ASSERT_EQ(it->first, key);
		ASSERT_EQ(it->second, val);
		++key, ++val;
		++it;
	}
};

TEST(map, reverse_iterator) {
	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};

	int key = 8, val = 8;
	auto it = mp.rbegin();
	while (it != mp.rend()) {
		ASSERT_EQ(it->first, key);
		ASSERT_EQ(it->second, val);
		--key, --val;
		++it;
	}
};

TEST(map, const_reverse_iterator) {
	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};

	int key = 8, val = 8;
	auto it = mp.crbegin();
	while (it != mp.crend()) {
		ASSERT_EQ(it->first, key);
		ASSERT_EQ(it->second, val);
		--key, --val;
		++it;
	}
};

TEST(map, empty) {
	map<int, int> mp1{
		{1,1}
	};
	ASSERT_EQ(mp1.empty(), false);

	map<int,int> mp2;
	ASSERT_EQ(mp2.empty(), true);
}

TEST(map, size) {
	map<int, int> mp1{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	ASSERT_EQ(mp1.size(), 8);

	map<int, int> mp2{
		{1,1},
		{1,2}
	};
	ASSERT_EQ(mp2.size(), 1);
}

TEST(map, clear) {
	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	ASSERT_EQ(mp.size(), 8);
	mp.clear();
	ASSERT_EQ(mp.size(), 0);
	auto it = mp.begin();
	ASSERT_EQ(it, mp.end());
}

TEST(map, insert_lvalue) {
	int len = 8;
	int nums[] = {1,2,3,4,5,6,7,8};

	map<int, int> mp;
	for (int i = 0; i < len; ++i) {
		pair<const int, int> p = make_pair(nums[i], nums[i]);
		auto ret = mp.insert(p);
		auto it = ret.first;
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
		ASSERT_EQ(ret.second, true);
		ASSERT_EQ(mp.size(), i + 1);
	}
	mp.clear();

	for (int i = len - 1; i >= 0; --i) {
		pair<const int, int> p = make_pair(nums[i], nums[i]);
		auto ret = mp.insert(p);
		auto it = ret.first;
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
		ASSERT_EQ(ret.second, true);
		ASSERT_EQ(mp.size(), len - i);
	}

	pair<const int, int> p = make_pair(1,1);
	auto ret = mp.insert(p);
	ASSERT_EQ(ret.first, mp.begin());
	ASSERT_EQ(ret.second, false);
}

TEST(map, insert_rvalue) {
	int len = 8;
	int nums[] = {1,2,3,4,5,6,7,8};

	map<int, int> mp;
	for (int i = 0; i < len; ++i) {
		auto ret = mp.insert(make_pair(nums[i], nums[i]));
		auto it = ret.first;
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
		ASSERT_EQ(ret.second, true);
		ASSERT_EQ(mp.size(), i + 1);
	}
	mp.clear();

	for (int i = len - 1; i >= 0; --i) {
		auto ret = mp.insert(make_pair(nums[i], nums[i]));
		auto it = ret.first;
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
		ASSERT_EQ(ret.second, true);
		ASSERT_EQ(mp.size(), len - i);
	}

	auto ret = mp.insert(make_pair(1,1));
	ASSERT_EQ(ret.first, mp.begin());
	ASSERT_EQ(ret.second, false);
}

// TODO: bug fix, erase leak memory
TEST(map, insert_and_erase) {
	int len = 8;
	int nums[] = {1,2,3,4,5,6,7,8};

	map<int, int> mp;
	for (int i = 0; i < len; ++i) {
		auto ret = mp.insert({nums[i], nums[i]});
		auto it = ret.first;
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
		ASSERT_EQ(ret.second, true);
		ASSERT_EQ(mp.size(), i + 1);
	}

	auto ret = mp.insert({1,1});
	ASSERT_EQ(ret.first, mp.find(1));
	ASSERT_EQ(ret.second, false);

	int i = 0;
	for (auto it = mp.begin(); it != mp.end(); ++it, ++i) {
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
	}

	mp.erase(mp.begin());
	i = 1;
	for (auto it = mp.begin(); it != mp.end(); ++it, ++i) {
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
	}
}
 
// TODO: bug fix, erase leak memory
TEST(map, emplace_and_erase) {
	int len = 8;
	int nums[] = {1,2,3,4,5,6,7,8};

	map<int, int> mp;
	for (int i = 0; i < len; ++i) {
		auto ret = mp.emplace(nums[i], nums[i]);
		auto it = ret.first;
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
		ASSERT_EQ(ret.second, true);
		ASSERT_EQ(mp.size(), i + 1);
	}

	int i = 0;
	for (auto it = mp.begin(); it != mp.end(); ++it, ++i) {
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
	}

	mp.erase(mp.begin());
	i = 1;
	for (auto it = mp.begin(); it != mp.end(); ++it, ++i) {
		ASSERT_EQ(it->first, nums[i]);
		ASSERT_EQ(it->second, nums[i]);
	}
}
 
TEST(map, count) {

	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	for (auto it = mp.begin(); it != mp.end(); ++it) {
		ASSERT_EQ(mp.count(it->first), 1);
	}
	ASSERT_EQ(mp.count(9), 0);
}

TEST(map, find) {

	map<int, int> mp{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	auto it1 = mp.find(1);
	ASSERT_EQ(it1, mp.begin());
	++it1;
	++it1;
	++it1;
	auto it2 = mp.find(4);
	ASSERT_EQ(it1, it2);
	auto it3 = mp.find(9);
	ASSERT_EQ(it3, mp.end());
}

TEST(map, lower_bound) {
	map<int, int> mp1{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	auto it = mp1.lower_bound(2);
	ASSERT_EQ(it->first, 2);
	it = mp1.lower_bound(9);
	ASSERT_EQ(it, mp1.end());

	const map<int, int> mp2{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	auto cit = mp2.lower_bound(2);
	ASSERT_EQ(cit->first, 2);
	cit = mp2.lower_bound(9);
	ASSERT_EQ(cit, mp2.cend());
}

TEST(map, upper_bound) {

	map<int, int> mp1{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	auto it = mp1.upper_bound(2);
	ASSERT_EQ(it->first, 3);
	it = mp1.upper_bound(8);
	ASSERT_EQ(it, mp1.end());

	const map<int, int> mp2{
		{1,1},
		{2,2},
		{3,3},
		{4,4},
		{5,5},
		{6,6},
		{7,7},
		{8,8},
	};
	auto cit = mp2.upper_bound(2);
	ASSERT_EQ(cit->first, 3);
	cit = mp2.upper_bound(8);
	ASSERT_EQ(cit, mp2.cend());
}
