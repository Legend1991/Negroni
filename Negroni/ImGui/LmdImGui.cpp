#include "LmdImGui.h"

#include "../Core/Input.h"
#include "../Core/Keyboard.h"
#include "../Core/Mouse.h"

ImGui::LmdImGui::LmdImGui(ImGuiPlatform& platform, ImGuiRenderer& renderer) :
	platform(platform), renderer(renderer), cleaned(false)
{
	IMGUI_CHECKVERSION();
}

ImGui::LmdImGui::~LmdImGui()
{
	Cleanup();
}

void ImGui::LmdImGui::Begin()
{
	renderer.Begin();
	platform.Begin();
	ImGui::NewFrame();
}

void ImGui::LmdImGui::End()
{
	ImGuiIO& io = ImGui::GetIO();

	Input::isKeyboardBlocked = io.WantCaptureKeyboard;
	Input::isMouseBlocked    = io.WantCaptureMouse;

	ImGui::Render();
	renderer.End();
}

void ImGui::LmdImGui::Cleanup()
{
	if (cleaned) return;

	renderer.Cleanup();
	platform.Cleanup();
	ImGui::DestroyContext();

	cleaned = true;
}
