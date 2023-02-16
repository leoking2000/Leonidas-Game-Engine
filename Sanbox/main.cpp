#include <iostream>
#include <LGE.h>
#include <IOC/Container.h>

class A
{
public:
	virtual i32 Test() { return 500; }
	virtual ~A() = default;
};

class B : public A
{
public:
	i32 Test() override { return 100; }
};

int main()
{
	using namespace LGE;

	IOC::Container::Get().RegisterFactory<A>([] { return std::make_unique<A>(); });
	IOC::Container::Get().RegisterFactory<B>([] { return std::make_unique<B>(); });

	auto a = IOC::Container::Get().CreateInstance<A>();
	auto b = IOC::Container::Get().CreateInstance<B>();

	std::cout << LGE::add(a->Test(), b->Test()) << std::endl; // 600

	IOC::Container::Get().RegisterFactory<A>([] { return std::make_unique<B>(); });

	a = IOC::Container::Get().CreateInstance<A>();
	std::cout << LGE::add(a->Test(), b->Test()) << std::endl; // 200

	return 0;
}