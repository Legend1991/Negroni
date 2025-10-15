#pragma once

#include "LmdImGui.h"
#include <windows.h>

using namespace Core;

namespace ImGui
{
	class ImGuiWin32Platform : public ImGuiPlatform
	{
	public:
		ImGuiWin32Platform(HWND hWnd);
		~ImGuiWin32Platform();

		virtual void Begin() override;
		virtual void End() override;
		virtual void Cleanup() override;

		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
