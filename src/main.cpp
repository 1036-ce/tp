#include <cassert>
#include <iostream>
#include <vector.hpp>

struct Data {
	Data(int _x, int _y) : x(_x), y(_y) {}
	int x;
	int y;
};

int main() {
	vector<int> vec{1,2,3,4};
	auto it = vec.rbegin();

	// operator++
	// operator!=
	// operator-
	while (it != vec.rend()) {
		// ASSERT_EQ(*it, vec.at(it - vec.rbegin()));
		std::cout << *it << std::endl;
		++it;
	}

	// operator>=
	// operator--
	// operator-
	it = vec.rend() - 1;
	while (it >= vec.rbegin()) {
		// ASSERT_EQ(*it, vec.at(it - vec.rbegin()));
		std::cout << *it << std::endl;
		--it;
	}

	// operator+
	it = vec.rbegin();
	// ASSERT_EQ(*(it + 2), 3);
	std::cout << *(it + 2) << std::endl;

	// operator-
	it = vec.rbegin() + 2;
	it = it - 2;
	std::cout << *it << std::endl;
	// ASSERT_EQ(*it, 1);
}
