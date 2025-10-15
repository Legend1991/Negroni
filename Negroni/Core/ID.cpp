#include "ID.h"

#include <random>
#include <set>

namespace Core
{
	static std::random_device randomDevice;
	static std::mt19937 engine(randomDevice());
	static std::uniform_int_distribution<uint32_t> uniformDistribution(1, 16777215);
	static std::set<uint32_t> usedIDs;

	ID::ID() : value(uniformDistribution(engine))
	{
		do
		{
			value = uniformDistribution(engine);
		}
		while (usedIDs.find(value) != usedIDs.end());

		usedIDs.insert(value);
	}

	ID::ID(uint32_t id) : value(id)
	{
	}

	const ID ID::None(0);
}