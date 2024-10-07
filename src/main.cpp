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
	deque<int> dq(5000);

	int a = 12;
	for (size_t i = 0; i < 8; ++i)
		dq.erase(dq.begin() + static_cast<size_t>(dq.size() * 0.7));
	dq.erase(dq.begin() + static_cast<size_t>(dq.size() * 0.7));
	a = 3;
}
