#include <cassert>
#include <iostream>
#include <list.hpp>

struct Data {
	Data() { std::cout << "Data()" << std::endl; }

	Data(const Data &) { std::cout << "copy ctor" << std::endl; }

	Data(Data &&) { std::cout << "move ctor" << std::endl; }

	~Data() { std::cout << "~Data()" << std::endl; }
};

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

int main() {
	std::initializer_list<int> il1{1,2,3,4,5};
	std::initializer_list<int> il2{1,2,3,4,5};
	list<int> lt(il1);

	auto it = lt.begin();
	lt.emplace(it, 9);

	it = inc_iter(lt.begin(), 2);
	lt.emplace(it, 9);

	it = inc_iter(lt.begin(), 4);
	lt.emplace(it, 9);

	it = lt.end();
	lt.emplace(it, 9);

	it = lt.begin();
	while (it != lt.end()) {
		std::cout << *it << std::endl;
		++it;
	}
}
