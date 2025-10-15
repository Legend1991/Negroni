#include "ImGuiDX11Renderer.h"

ImGui::ImGuiDX11Renderer::ImGuiDX11Renderer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if (!ImGui::GetCurrentContext())
		ImGui::CreateContext();
	ImGui_ImplDX11_Init(device, deviceContext);
}

ImGui::ImGuiDX11Renderer::~ImGuiDX11Renderer()
{
	Cleanup();
}

void ImGui::ImGuiDX11Renderer::Begin()
{
	ImGui_ImplDX11_NewFrame();
}

void ImGui::ImGuiDX11Renderer::End()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGui::ImGuiDX11Renderer::Cleanup()
{
	if (!ImGui::GetCurrentContext() || !ImGui::GetIO().BackendRendererUserData)
		return;
	ImGui_ImplDX11_Shutdown();
}
