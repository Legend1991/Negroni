#pragma once

#include "../Core/Window.h"

#include <string>
#include <windows.h>

using namespace Core;

namespace Windows
{
	class Win32Window : public Window
	{
	public:
		Win32Window(size_t width, size_t height, std::wstring name);
		~Win32Window();

		virtual void Show() override;
		virtual InputEvent ReadInput() override;
		virtual bool Closed() override { return closed; }
		virtual void Cleanup() override;

		void Resize(size_t width, size_t height);

		operator HWND() const { return hWnd; }

		std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> OnWndProc;

	private:
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void SetColorTheme();
		bool HandleInput(UINT msg, WPARAM wParam, LPARAM lParam);
		Key KeyCodeToInputKey(WPARAM wParam, LPARAM lParam);

		WNDCLASSEXW wndClass;
		HWND hWnd;
		HMODULE hUxtheme;

		size_t width;
		size_t height;
		bool closed;

		InputEvent input;
	};
}
