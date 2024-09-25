#include <cassert>
#include <iostream>
#include <shared_ptr.hpp>
#include <unique_ptr.hpp>

void *operator new(size_t n) {
	std::cout << "operator new" << std::endl;
	return malloc(n);
}

void operator delete(void *p) noexcept {
	std::cout << "operator delete" << std::endl;
	free(p);
}

class Good : public enable_shared_from_this<Good> {
	public:

		static shared_ptr<Good> create() {
			return shared_ptr<Good>(new Good());
		}

		void fun() {
			std::cout << "Good::fun()" << std::endl;
		}

		shared_ptr<Good> getptr() {
			return shared_from_this();
		}
	private:
		Good() = default;
};

struct Foo {
    Foo(int _val) : val(_val) { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
    std::string print() { return std::to_string(val); }
    int val;
};
 
int main()
{

	unique_ptr<int> u1(new int(1));

	u1.reset();
	std::cout << (u1.get() == nullptr) << std::endl;

	unique_ptr<int> u2(new int(1));

	u2.reset(new int(2));
	std::cout << (u1.get() == nullptr) << std::endl;
}
