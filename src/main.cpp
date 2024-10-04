#include <cassert>
#include <deque.hpp>
#include <iostream>

struct Data {
	Data() { std::cout << "Data()" << std::endl; }

	Data(const Data &) { std::cout << "copy ctor" << std::endl; }

	Data(Data &&) { std::cout << "move ctor" << std::endl; }

	~Data() { std::cout << "~Data()" << std::endl; }
};

int main() {
	deque<int> dq{1,2,3,4,5};
    auto it = dq.erase(dq.begin() + 2, dq.begin() + 3);
	std::cout << *it << std::endl;
	std::cout << "=====================" << std::endl;

	it = dq.begin();
	while (it != dq.end()) {
		std::cout << *it << std::endl;
		++it;
    } 
}
