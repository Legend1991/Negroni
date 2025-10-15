#include "Win32Window.h"
#include "Windowsx.h"

#include "../Core/Logger.h"
#include "../Core/Input.h"
#include "../Core/Keyboard.h"
#include "../Core/Mouse.h"
#include "../resource.h"

Windows::Win32Window::Win32Window(size_t width, size_t height, std::wstring name) :
    width(width), height(height), closed(false)
{
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_CLASSDC;
    wndClass.lpfnWndProc = Windows::Win32Window::WndProc;
    wndClass.cbClsExtra = 0L;
    wndClass.cbWndExtra = 0L;
    wndClass.hInstance = GetModuleHandle(nullptr);
    wndClass.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    wndClass.hCursor = nullptr;
    wndClass.hbrBackground = nullptr;
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = name.c_str();
    wndClass.hIconSm = nullptr;

    RegisterClassExW(&wndClass);
    hWnd = CreateWindowW(wndClass.lpszClassName, name.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, width, height,
                           nullptr, nullptr, wndClass.hInstance, nullptr);
    SetPropW(hWnd, L"Win32Window", (HANDLE)this);
    SetColorTheme();
}

Windows::Win32Window::~Win32Window()
{
    Cleanup();
    FreeLibrary(hUxtheme);
}

void Windows::Win32Window::SetColorTheme()
{
    enum class WINDOWCOMPOSITIONATTRIB
    {
        WCA_UNDEFINED = 0,
        WCA_NCRENDERING_ENABLED = 1,
        WCA_NCRENDERING_POLICY = 2,
        WCA_TRANSITIONS_FORCEDISABLED = 3,
        WCA_ALLOW_NCPAINT = 4,
        WCA_CAPTION_BUTTON_BOUNDS = 5,
        WCA_NONCLIENT_RTL_LAYOUT = 6,
        WCA_FORCE_ICONIC_REPRESENTATION = 7,
        WCA_EXTENDED_FRAME_BOUNDS = 8,
        WCA_HAS_ICONIC_BITMAP = 9,
        WCA_THEME_ATTRIBUTES = 10,
        WCA_NCRENDERING_EXILED = 11,
        WCA_NCADORNMENTINFO = 12,
        WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
        WCA_VIDEO_OVERLAY_ACTIVE = 14,
        WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
        WCA_DISALLOW_PEEK = 16,
        WCA_CLOAK = 17,
        WCA_CLOAKED = 18,
        WCA_ACCENT_POLICY = 19,
        WCA_FREEZE_REPRESENTATION = 20,
        WCA_EVER_UNCLOAKED = 21,
        WCA_VISUAL_OWNER = 22,
        WCA_HOLOGRAPHIC = 23,
        WCA_EXCLUDED_FROM_DDA = 24,
        WCA_PASSIVEUPDATEMODE = 25,
        WCA_USEDARKMODECOLORS = 26,
        WCA_LAST = 27
    };

    struct WINDOWCOMPOSITIONATTRIBDATA
    {
        WINDOWCOMPOSITIONATTRIB Attrib;
        PVOID pvData;
        SIZE_T cbData;
    };

    enum class PreferredAppMode
    {
        Default,
        AllowDark,
        ForceDark,
        ForceLight,
        Max
    };

    using fnShouldAppsUseDarkMode = bool (WINAPI*)();
    using fnAllowDarkModeForWindow = bool (WINAPI*)(HWND hWnd, bool allow);
    using fnSetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode appMode);
    using fnSetWindowCompositionAttribute = BOOL(WINAPI*)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA*);

    hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hUxtheme == 0)
    {
        log_warn("can't load uxtheme.dll");
        return;
    }

    fnShouldAppsUseDarkMode ShouldAppsUseDarkMode;
    ShouldAppsUseDarkMode = (fnShouldAppsUseDarkMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132));
    fnAllowDarkModeForWindow AllowDarkModeForWindow;
    AllowDarkModeForWindow = (fnAllowDarkModeForWindow)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133));
    fnSetPreferredAppMode SetPreferredAppMode;
    SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    fnSetWindowCompositionAttribute SetWindowCompositionAttribute;

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32 == 0)
    {
        log_warn("can't get module handle for user32.dll");
        return;
    }

    SetWindowCompositionAttribute = (fnSetWindowCompositionAttribute)(GetProcAddress(hUser32, "SetWindowCompositionAttribute"));

    BOOL dark = ShouldAppsUseDarkMode();
    log_info("color theme: {}", dark ? "Dark" : "Light");
    SetPropW(hWnd, L"UseImmersiveDarkModeColors", reinterpret_cast<HANDLE>(static_cast<INT_PTR>(dark)));
    AllowDarkModeForWindow(hWnd, true);
    SetPreferredAppMode(PreferredAppMode::AllowDark);
    WINDOWCOMPOSITIONATTRIBDATA data = { WINDOWCOMPOSITIONATTRIB::WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
    SetWindowCompositionAttribute(hWnd, &data);
}

void Windows::Win32Window::Show()
{
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
}

// Poll and handle messages (inputs, window resize, etc.)
// See the WndProc() function below for our to dispatch events to the Win32 backend.
InputEvent Windows::Win32Window::ReadInput()
{
    input.mouseScrollDelta = 0;
    input.isDirty = false;

    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
            closed = true;
    }

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    OnResized(width, height);

    return input;
}

void Windows::Win32Window::Cleanup()
{
    DestroyWindow(hWnd);
    UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);
}

void Windows::Win32Window::Resize(size_t newWidth, size_t newHeight)
{
    log_debug("resize to {} x {}", newWidth, newHeight);

    width = newWidth;
    height = newHeight;
}

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI Windows::Win32Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto window = (Win32Window*)GetPropW(hWnd, L"Win32Window");
    if (window != nullptr && window->OnWndProc && window->OnWndProc(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    if (window)
    {
        if (window->HandleInput(msg, wParam, lParam))
        {
            window->input.isDirty = true;
            return 0;
        }
    }

    switch (msg)
    {
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
            return 0;
        if (window == nullptr)
        {
            log_error("Win32Window not found");
            return 0;
        }
        window->Resize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
    } return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    } return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool Windows::Win32Window::HandleInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!Input::isKeyboardBlocked)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            if (wParam < Core::KEYS_IN_USE_LENGTH)
            {
                Key key = KeyCodeToInputKey(wParam, lParam);
                if (key != Key::None)
                {
                    bool isKeyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
                    input.keysInUse[key] = isKeyDown;
                }
            }
        } return true;
        }
    }

    if (!Input::isMouseBlocked)
    {
        switch (msg)
        {
        case WM_MOUSEMOVE:
        {
            input.mousePositionX = GET_X_LPARAM(lParam);
            input.mousePositionY = GET_Y_LPARAM(lParam);
        } return true;
        case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
        {
            Key key = Key::None;
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { key = Key::MouseLeft; }
            if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { key = Key::MouseRight; }
            if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { key = Key::MouseMiddle; }
            if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK)
            { 
                key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Key::MouseX1 : Key::MouseX2;
            }
            input.keysInUse[key] = true;
        } return true;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            Key key = Key::None;
            if (msg == WM_LBUTTONUP) { key = Key::MouseLeft; }
            if (msg == WM_RBUTTONUP) { key = Key::MouseRight; }
            if (msg == WM_MBUTTONUP) { key = Key::MouseMiddle; }
            if (msg == WM_XBUTTONUP)
            {
                key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Key::MouseX1 : Key::MouseX2;
            }
            input.keysInUse[key] = false;
        } return true;
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        {
            input.mouseScrollDelta += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        } return true;
        }
    }

    return false;
}

Key Windows::Win32Window::KeyCodeToInputKey(WPARAM wParam, LPARAM lParam)
{
    // There is no distinct VK_xxx for keypad enter, instead it is VK_RETURN + KF_EXTENDED.
    if ((wParam == VK_RETURN) && (HIWORD(lParam) & KF_EXTENDED))
        return KeypadEnter;

    // Virtual-Key Codes: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    // Also this is usefull: https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input
    switch (wParam)
    {
    case VK_TAB: return Key::Tab;
    case VK_LEFT: return Key::LeftArrow;
    case VK_RIGHT: return Key::RightArrow;
    case VK_UP: return Key::UpArrow;
    case VK_DOWN: return Key::DownArrow;
    case VK_PRIOR: return Key::PageUp;
    case VK_NEXT: return Key::PageDown;
    case VK_HOME: return Key::Home;
    case VK_END: return Key::End;
    case VK_INSERT: return Key::Insert;
    case VK_DELETE: return Key::Delete;
    case VK_BACK: return Key::Backspace;
    case VK_SPACE: return Key::Space;
    case VK_RETURN: return Key::Enter;
    case VK_ESCAPE: return Key::Escape;
        //case VK_OEM_7: return Key::Apostrophe;
    case VK_OEM_COMMA: return Key::Comma;
        //case VK_OEM_MINUS: return Key::Minus;
    case VK_OEM_PERIOD: return Key::Period;
        //case VK_OEM_2: return Key::Slash;
        //case VK_OEM_1: return Key::Semicolon;
        //case VK_OEM_PLUS: return Key::Equal;
        //case VK_OEM_4: return Key::LeftBracket;
        //case VK_OEM_5: return Key::Backslash;
        //case VK_OEM_6: return Key::RightBracket;
        //case VK_OEM_3: return Key::GraveAccent;
    case VK_CAPITAL: return Key::CapsLock;
    case VK_SCROLL: return Key::ScrollLock;
    case VK_NUMLOCK: return Key::NumLock;
    case VK_SNAPSHOT: return Key::PrintScreen;
    case VK_PAUSE: return Key::Pause;
    case VK_NUMPAD0: return Key::Keypad0;
    case VK_NUMPAD1: return Key::Keypad1;
    case VK_NUMPAD2: return Key::Keypad2;
    case VK_NUMPAD3: return Key::Keypad3;
    case VK_NUMPAD4: return Key::Keypad4;
    case VK_NUMPAD5: return Key::Keypad5;
    case VK_NUMPAD6: return Key::Keypad6;
    case VK_NUMPAD7: return Key::Keypad7;
    case VK_NUMPAD8: return Key::Keypad8;
    case VK_NUMPAD9: return Key::Keypad9;
    case VK_DECIMAL: return Key::KeypadDecimal;
    case VK_DIVIDE: return Key::KeypadDivide;
    case VK_MULTIPLY: return Key::KeypadMultiply;
    case VK_SUBTRACT: return Key::KeypadSubtract;
    case VK_ADD: return Key::KeypadAdd;
    case VK_LSHIFT: return Key::LeftShift;
    case VK_LCONTROL: return Key::LeftCtrl;
    case VK_LMENU: return Key::LeftAlt;
    case VK_LWIN: return Key::LeftSuper;
    case VK_RSHIFT: return Key::RightShift;
    case VK_RCONTROL: return Key::RightCtrl;
    case VK_RMENU: return Key::RightAlt;
    case VK_RWIN: return Key::RightSuper;
    case VK_SHIFT: return Key::Shift;
    case VK_CONTROL: return Key::Ctrl;
    case VK_MENU: return Key::Alt;
    case VK_APPS: return Key::Menu;
    case '0': return Key::Num0;
    case '1': return Key::Num1;
    case '2': return Key::Num2;
    case '3': return Key::Num3;
    case '4': return Key::Num4;
    case '5': return Key::Num5;
    case '6': return Key::Num6;
    case '7': return Key::Num7;
    case '8': return Key::Num8;
    case '9': return Key::Num9;
    case 'A': return Key::A;
    case 'B': return Key::B;
    case 'C': return Key::C;
    case 'D': return Key::D;
    case 'E': return Key::E;
    case 'F': return Key::F;
    case 'G': return Key::G;
    case 'H': return Key::H;
    case 'I': return Key::I;
    case 'J': return Key::J;
    case 'K': return Key::K;
    case 'L': return Key::L;
    case 'M': return Key::M;
    case 'N': return Key::N;
    case 'O': return Key::O;
    case 'P': return Key::P;
    case 'Q': return Key::Q;
    case 'R': return Key::R;
    case 'S': return Key::S;
    case 'T': return Key::T;
    case 'U': return Key::U;
    case 'V': return Key::V;
    case 'W': return Key::W;
    case 'X': return Key::X;
    case 'Y': return Key::Y;
    case 'Z': return Key::Z;
    case VK_F1: return Key::F1;
    case VK_F2: return Key::F2;
    case VK_F3: return Key::F3;
    case VK_F4: return Key::F4;
    case VK_F5: return Key::F5;
    case VK_F6: return Key::F6;
    case VK_F7: return Key::F7;
    case VK_F8: return Key::F8;
    case VK_F9: return Key::F9;
    case VK_F10: return Key::F10;
    case VK_F11: return Key::F11;
    case VK_F12: return Key::F12;
    case VK_F13: return Key::F13;
    case VK_F14: return Key::F14;
    case VK_F15: return Key::F15;
    case VK_F16: return Key::F16;
    case VK_F17: return Key::F17;
    case VK_F18: return Key::F18;
    case VK_F19: return Key::F19;
    case VK_F20: return Key::F20;
    case VK_F21: return Key::F21;
    case VK_F22: return Key::F22;
    case VK_F23: return Key::F23;
    case VK_F24: return Key::F24;
    case VK_BROWSER_BACK: return Key::AppBack;
    case VK_BROWSER_FORWARD: return Key::AppForward;
    default: break;
    }

    const int scancode = (int)LOBYTE(HIWORD(lParam));

    // Fallback to scancode
    // https://handmade.network/forums/t/2011-keyboard_inputs_-_scancodes,_raw_input,_text_input,_key_names
    switch (scancode)
    {
    case 41: return Key::GraveAccent;  // VK_OEM_8 in EN-UK, VK_OEM_3 in EN-US, VK_OEM_7 in FR, VK_OEM_5 in DE, etc.
    case 12: return Key::Minus;
    case 13: return Key::Equal;
    case 26: return Key::LeftBracket;
    case 27: return Key::RightBracket;
    case 86: return Key::Oem102;
    case 43: return Key::Backslash;
    case 39: return Key::Semicolon;
    case 40: return Key::Apostrophe;
    case 51: return Key::Comma;
    case 52: return Key::Period;
    case 53: return Key::Slash;
    default: break;
    }

    return Key::None;
}
