#pragma once

#include "Base.h"

#include <utility>

namespace Core
{
	template<typename ValueType>
	class SwapChain
	{
	public:
		void write(const ValueType& value)
		{
			std::lock_guard<std::mutex> guard(key);
			buffers[backIndex] = value;
			std::swap(frontIndex, backIndex);
		}

		ValueType read()
		{
			std::lock_guard<std::mutex> guard(key);
			return buffers[frontIndex];
		}

	private:
		std::mutex key;

		ValueType buffers[2];
		i8 frontIndex = 0;
		i8 backIndex = 1;
	};
}
