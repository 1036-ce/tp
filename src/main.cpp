#include <cassert>
#include <deque.hpp>
#include <iostream>

struct Data {
	Data() { std::cout << "Data()" << std::endl; }

	~Data() { std::cout << "~Data()" << std::endl; }
};

int main() {
	deque<int> dq;
	for (int i = 0; i < 1000; ++i)
		dq.push_front(i);

	dq.push_front(0);


	auto it = dq.begin();
	while (it != dq.end()) {
		std::cout << *it << std::endl;
		++it;
	} 
}
