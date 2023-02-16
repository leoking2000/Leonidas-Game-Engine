#include "pch.h"

/////////////////////////////
//       mock code         //
/////////////////////////////
#include <string>
#include <memory>

class D
{
public:
	virtual std::string Process() { return "D"; }
	virtual ~D() = default;
};

class C
{
public:
	C(std::unique_ptr<D> d)
		:
		m_d(std::move(d))
	{
	}

	virtual std::string Process() { return m_d->Process() + "-C"; }
	virtual ~C() = default;
protected:
	std::unique_ptr<D> m_d;
};

class B
{
public:
	B(std::unique_ptr<C> c)
		:
		m_c(std::move(c))
	{
	}

	virtual std::string Process() { return m_c->Process() + "-B"; }
	virtual ~B() = default;
protected:
	std::unique_ptr<C> m_c;
};

class A
{
public:
	A(std::unique_ptr<B> b)
		:
		m_b(std::move(b))
	{
	}

	virtual std::string Process() { return m_b->Process() + "-A"; }
	virtual ~A() = default;
protected:
	std::unique_ptr<B> m_b;
};



class D_Better : public D
{
public:
	std::string Process() override { return "D_Better"; }
};

class B_Better : public B
{
public:
	B_Better(std::unique_ptr<C> c)
		:
		B(std::move(c))
	{
	}

	std::string Process() override { return m_c->Process() + "-B_Better"; }
};


/////////////////////////////
//       test code         //
/////////////////////////////
#include <IOC/Container.h>
using namespace LGE;

#pragma warning (push)
#pragma warning (disable : 26433)

class IOCContainerTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		m_container = new IOC::Container();

		m_container->RegisterFactory<D>([&]() { return std::make_unique<D>(); });

		m_container->RegisterFactory<C>([&]() { 
			return std::make_unique<C>(m_container->CreateInstance<D>()); 
		});

		m_container->RegisterFactory<B>([&]() {
			return std::make_unique<B>(m_container->CreateInstance<C>());
		});

		m_container->RegisterFactory<A>([&]() {
			return std::make_unique<A>(m_container->CreateInstance<B>());
		});
	}

	void TearDown() override
	{
		delete m_container;
	}

	IOC::Container* m_container = nullptr;
};

TEST_F(IOCContainerTest, normal_behavior_test)
{
	auto a = m_container->CreateInstance<A>();
	ASSERT_EQ(a->Process(), "D-C-B-A");
}

TEST_F(IOCContainerTest, change_D_test)
{
	m_container->RegisterFactory<D>([&]() { return std::make_unique<D_Better>(); });

	auto a = m_container->CreateInstance<A>();
	ASSERT_EQ(a->Process(), "D_Better-C-B-A");
}

TEST_F(IOCContainerTest, change_C_test)
{
	m_container->RegisterFactory<B>([&]() {
		return std::make_unique<B_Better>(m_container->CreateInstance<C>());
	});

	auto a = m_container->CreateInstance<A>();
	ASSERT_EQ(a->Process(), "D-C-B_Better-A");
}

TEST_F(IOCContainerTest, change_D_C_test)
{
	m_container->RegisterFactory<D>([&]() { return std::make_unique<D_Better>(); });

	m_container->RegisterFactory<B>([&]() {
		return std::make_unique<B_Better>(m_container->CreateInstance<C>());
	});

	auto a = m_container->CreateInstance<A>();
	ASSERT_EQ(a->Process(), "D_Better-C-B_Better-A");
}


#pragma warning (pop)