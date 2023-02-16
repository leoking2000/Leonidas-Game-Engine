#pragma once
#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include <memory>
#include <execution>

namespace LGE::IOC
{
	class Container
	{
	public:
		template<class T>
		using FactoryFunction = std::function<std::unique_ptr<T>()>;
		
		template<class T>
		void RegisterFactory(FactoryFunction<T> factory)
		{
			m_factory_map[typeid(T)] = factory;
		}

		template<class T>
		std::unique_ptr<T> CreateInstance()
		{
			const auto i = m_factory_map.find(typeid(T));
			if (i == m_factory_map.end())
			{
				throw std::runtime_error(std::string("Factory for type ") + typeid(T).name() + std::string(" does not exist!!!"));
			}

			return std::move(std::any_cast<FactoryFunction<T>>(i->second)());
		}


		static Container& Get() noexcept;
	private:
		std::unordered_map<std::type_index, std::any> m_factory_map;
	};
}
