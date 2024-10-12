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
	list<int> lt1{1, 2, 3, 4};
	list<int> lt2{4,3,2,1,4,5,6,1,2,12,};

	lt1.swap(lt2);

	auto it = lt1.begin();
	while (it != lt1.end()) {
		std::cout << *it << std::endl;
		++it;
	}
}
