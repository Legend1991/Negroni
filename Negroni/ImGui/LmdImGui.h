#pragma once

#include "../Core/Base.h"
#include "../Core/Gui.h"
#include "imgui.h"

using namespace Core;

namespace ImGui
{
	class ImGuiAdapter
	{
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Cleanup() = 0;
	};

	class ImGuiPlatform : public ImGuiAdapter {};
	class ImGuiRenderer : public ImGuiAdapter {};

	class LmdImGui : public Gui
	{
	public:
		LmdImGui(ImGuiPlatform& platform, ImGuiRenderer& renderer);
		~LmdImGui();

		virtual void Begin() override;
		virtual void End() override;
		virtual void Cleanup() override;

	private:
		ImGuiPlatform& platform;
		ImGuiRenderer& renderer;

		bool cleaned;
	};
}
