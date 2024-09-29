#include <vector.hpp>
#include <gtest/gtest.h>

TEST(vector, iterator) {
	vector<int> vec{1,2,3,4};
	auto it = vec.begin();

	// operator++
	// operator!=
	// operator-
	while (it != vec.end()) {
		ASSERT_EQ(*it, vec.at(it - vec.begin()));
		++it;
	}

	// operator>=
	// operator--
	// operator-
	it = vec.end() - 1;
	while (it >= vec.begin()) {
		ASSERT_EQ(*it, vec.at(it - vec.begin()));
		--it;
	}

	// operator+
	it = vec.begin();
	ASSERT_EQ(*(it + 2), 3);

	// operator-
	it = vec.begin() + 2;
	it = it - 2;
	ASSERT_EQ(*it, 1);
}

TEST(vector, reverse_iterator) {
	vector<int> vec{1,2,3,4};
	auto it = vec.rbegin();

	// operator++
	// operator!=
	// operator-
	int pos = vec.size() - 1;
	while (it != vec.rend()) {
		ASSERT_EQ(*it, vec.at(pos));
		++it;
		--pos;
	}

	// operator>=
	// operator--
	// operator-
	pos = 0;
	it = vec.rend() - 1;
	while (it >= vec.rbegin()) {
		ASSERT_EQ(*it, vec.at(pos));
		--it;
		++pos;
	}

	// operator+
	it = vec.rbegin();
	ASSERT_EQ(*(it + 2), 2);

	// operator-
	it = vec.rbegin() + 2;
	it = it - 2;
	ASSERT_EQ(*it, 4);
}

TEST(vector, insert_1) {
	vector<int> vec{1,2,3,4};
	vector<int> target{1,2,2,3,4};

	int a = 2;

	auto it = vec.insert(vec.begin() + 1, a);
	ASSERT_EQ(*it, 2);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}

TEST(vector, insert_2) {
	vector<int> vec{1,2,3,4};
	vector<int> target{1,2,2,3,4};

	auto it = vec.insert(vec.begin() + 1, 2);
	ASSERT_EQ(*it, 2);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}

TEST(vector, insert_3) {
	vector<int> vec{1,2,3,4};
	vector<int> target{1,5,5,5,2,3,4};

	auto it = vec.insert(vec.begin() + 1, 3, 5);
	ASSERT_EQ(*it, 5);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}

TEST(vector, insert_4) {
	vector<int> vec{1,2,3,4};
	vector<int> nums{5,4,3,2};
	vector<int> target{1,5,4,3,2,3,4};

	auto it = vec.insert(vec.begin() + 1, nums.begin(), nums.begin() + 3);
	ASSERT_EQ(*it, 5);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}

TEST(vector, insert_5) {
	vector<int> vec{1,2,3,4};
	vector<int> target{1,5,4,3,2,3,4};

	auto it = vec.insert(vec.begin() + 1, {5,4,3});
	ASSERT_EQ(*it, 5);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}

TEST(vector, erase_1) {
	vector<int> vec{1,2,3,4,5};
	vector<int> target{1,3,4,5};

	auto it = vec.erase(vec.begin() + 1);
	ASSERT_EQ(*it, 3);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}

TEST(vector, erase_2) {
	vector<int> vec{1,2,3,4,5};
	vector<int> target{1,5};

	auto it = vec.erase(vec.begin() + 1, vec.begin() + 4);
	ASSERT_EQ(*it, 5);

	it = vec.begin();
	while (it != vec.end()) {
		ASSERT_EQ(*it, target.at(it - vec.begin()));
		++it;
	}
}
