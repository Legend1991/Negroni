#pragma once

#include <stdint.h>
#include <cstddef>

namespace Core
{
	class ID
	{
	public:
		ID();
		ID(uint32_t value);
		ID(const ID&) = default;

		static const ID None;

		operator uint32_t() const { return value; }
	private:
		uint32_t value = 1;
	};
}

namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<Core::ID>
	{
		std::size_t operator()(const Core::ID& id) const
		{
			return (uint32_t)id;
		}
	};
}
