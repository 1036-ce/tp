#include <cassert>
#include <shared_ptr.hpp>
#include <map.hpp>
#include <iostream>

int main() {
	shared_ptr<int> sp1(new int(1));
	auto sp2 = sp1;
	std::cout << sp1.use_count() << std::endl;
	std::cout << sp2.use_count() << std::endl;
}
