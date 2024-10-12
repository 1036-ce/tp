#include <cassert>
#include <iostream>
#include <list.hpp>

struct Data {
	Data() { std::cout << "Data()" << std::endl; }

	Data(const Data &) { std::cout << "copy ctor" << std::endl; }

	Data(Data &&) { std::cout << "move ctor" << std::endl; }

	~Data() { std::cout << "~Data()" << std::endl; }
};

inline auto inc_iter(list<int>::iterator it, std::size_t num) -> decltype(it) {
	while (num--)
		++it;
	return it;
}

inline auto dec_iter(list<int>::iterator it, std::size_t num) -> decltype(it) {
	while (num--)
		--it;
	return it;
}

int main() {
	list<int> lt{1, 2, 3, 4};

	lt.push_front(9);
	// lt.pop_back();

	auto it = lt.begin();
	while (it != lt.end()) {
		std::cout << *it << std::endl;
		++it;
	}
}
