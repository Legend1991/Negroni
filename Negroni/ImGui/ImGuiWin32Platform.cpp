#include "ImGuiWin32Platform.h"

#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImGui::ImGuiWin32Platform::ImGuiWin32Platform(HWND hWnd)
{
	if (!ImGui::GetCurrentContext())
		ImGui::CreateContext();
	ImGui_ImplWin32_EnableDpiAwareness();
	ImGui_ImplWin32_Init(hWnd);
}

ImGui::ImGuiWin32Platform::~ImGuiWin32Platform()
{
	Cleanup();
}

void ImGui::ImGuiWin32Platform::Begin()
{
	ImGui_ImplWin32_NewFrame();
}

void ImGui::ImGuiWin32Platform::End()
{
}

void ImGui::ImGuiWin32Platform::Cleanup()
{
	if (!ImGui::GetCurrentContext() || !ImGui::GetIO().BackendPlatformUserData)
		return;
	ImGui_ImplWin32_Shutdown();
}

LRESULT ImGui::ImGuiWin32Platform::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}
