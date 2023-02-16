#include "Container.h"

namespace LGE::IOC
{
	Container& Container::Get() noexcept
	{
		static Container container;
		return container;
	}
}
