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
	deque<int> dq{1,2,3,4};
	auto it1 = dq.rend();
	auto it2 = dq.rbegin();
	std::cout << (it1 - it2) << std::endl;
}
