#pragma once

#include "LmdImGui.h"
#include "imgui_impl_dx11.h"

using namespace Core;

namespace ImGui
{
	class ImGuiDX11Renderer : public ImGuiRenderer
	{
	public:
		ImGuiDX11Renderer(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		~ImGuiDX11Renderer();

		virtual void Begin() override;
		virtual void End() override;
		virtual void Cleanup() override;
	};
}
