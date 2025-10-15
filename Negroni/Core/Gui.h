#pragma once

#include "Base.h"
#include "Profiler.h"

#include <deque>

namespace Core
{
	class GuiLayer
	{
	public:
		virtual void Draw() = 0;

		bool visible = true;
	};

	class Gui
	{
	public:
		void Add(GuiLayer* layer) { layers.push_back(layer); }

		void Draw()
		{
			ProfileBlock("[Gui] Draw");
			Begin();
			for (auto& layer : layers)
				if (layer->visible) layer->Draw();
			End();
		}

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Cleanup() = 0;

	private:
		std::deque<GuiLayer*> layers;
	};
}
