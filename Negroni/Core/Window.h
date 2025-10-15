#pragma once

#include "Base.h"
#include "Input.h"

#include <functional>
#include <vector>

namespace Core
{
	class Window
	{
	public:
		virtual void Show() = 0;
		virtual InputEvent ReadInput() = 0;
		virtual bool Closed() = 0;
		virtual void Cleanup() = 0;

		std::function<void(size_t, size_t)> OnResized;
	};
}
