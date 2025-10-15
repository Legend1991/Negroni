#pragma once

#include "../Core/Gui.h"
#include "../Core/Renderer.h"
#include "imgui.h"

#include <functional>

using namespace Core;

namespace ImGui
{
	class ImGuiDemo : public GuiLayer
	{
	public:
		ImGuiDemo(Renderer& renderer);
		~ImGuiDemo();

		virtual void Draw() override;

	private:
		void SetupStyle();

		Renderer& renderer;

		bool vSync = true;
		bool newVSync = true;

		bool fxaa = true;
		bool newFXAA = true;
	};
}
