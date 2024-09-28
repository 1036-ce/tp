#include <cassert>
#include <iostream>
#include <vector.hpp>

struct Data {
	Data(int _x, int _y): x(_x), y(_y) {}
	int x;
	int y;
};

int main() {
	vector<int> vec{1,2,3,4,5,6,7,8,9,10};
	auto it = vec.erase(vec.cbegin()+1, vec.cend() - 4);

	for (int i = 0; i < vec.size(); ++i)
		std::cout << vec.at(i) << std::endl;

	std::cout << "===================" << std::endl;
	std::cout << *it << std::endl;
}
