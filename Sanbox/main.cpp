#include <iostream>
#include <LGE.h>

class A
{
public:
	virtual i32 Test() { return 500; }
	virtual ~A() = default;
};

class B : public A
{
public:
	i32 Test() override { return 400; }
};

int main()
{
	A a;
	B b;

	std::cout << LGE::add(a.Test(), b.Test()) << std::endl;

	return 0;
}