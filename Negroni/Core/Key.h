#pragma once

#include "Base.h"

#include <map>

namespace Core
{
    enum Key : u8
    {
        // Keyboard
        None = 0,
        Tab = 1,
        LeftArrow,
        RightArrow,
        UpArrow,
        DownArrow,
        PageUp,
        PageDown,
        Home,
        End,
        Insert,
        Delete,
        Backspace,
        Space,
        Enter,
        Escape,
        LeftCtrl, LeftShift, LeftAlt, LeftSuper,
        RightCtrl, RightShift, RightAlt, RightSuper,
        Shift,
        Ctrl,  // Ctrl (non-macOS), Cmd (macOS)
        Alt,   // Option/Menu
        Super, // Windows/Super (non-macOS), Ctrl (macOS)
        Menu,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        A, B, C, D, E, F, G, H, I, J,
        K, L, M, N, O, P, Q, R, S, T,
        U, V, W, X, Y, Z,
        F1, F2, F3, F4, F5, F6,
        F7, F8, F9, F10, F11, F12,
        F13, F14, F15, F16, F17, F18,
        F19, F20, F21, F22, F23, F24,
        Apostrophe,        // '
        Comma,             // ,
        Minus,             // -
        Period,            // .
        Slash,             // /
        Semicolon,         // ;
        Equal,             // =
        LeftBracket,       // [
        Backslash,         // \ (this text inhibit multiline comment caused by backslash)
        RightBracket,      // ]
        GraveAccent,       // `
        // Tilde,          // No need to uncomment. It's just an alias for GraveAccent key, handled by StringToKey function
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        Keypad0, Keypad1, Keypad2, Keypad3, Keypad4,
        Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
        KeypadDecimal,
        KeypadDivide,
        KeypadMultiply,
        KeypadSubtract,
        KeypadAdd,
        KeypadEnter,
        KeypadEqual,
        AppBack,               // Available on some keyboard/mouses. Often referred as "Browser Back"
        AppForward,
        Oem102,                // Non-US backslash.

        // Gamepad (not implemented yet)
        // (analog values are 0.0f to 1.0f)
        // (download controller mapping PNG/PSD at http://dearimgui.com/controls_sheets)
        //                     // XBOX        | SWITCH  | PLAYSTA. | -> ACTION
        GamepadStart,          // Menu        | +       | Options  |
        GamepadBack,           // View        | -       | Share    |
        GamepadFaceLeft,       // X           | Y       | Square   | Tap: Toggle Menu. Hold: Windowing mode (Focus/Move/Resize windows)
        GamepadFaceRight,      // B           | A       | Circle   | Cancel / Close / Exit
        GamepadFaceUp,         // Y           | X       | Triangle | Text Input / On-screen Keyboard
        GamepadFaceDown,       // A           | B       | Cross    | Activate / Open / Toggle / Tweak
        GamepadDpadLeft,       // D-pad Left  | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
        GamepadDpadRight,      // D-pad Right | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
        GamepadDpadUp,         // D-pad Up    | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
        GamepadDpadDown,       // D-pad Down  | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
        GamepadL1,             // L Bumper    | L       | L1       | Tweak Slower / Focus Previous (in Windowing mode)
        GamepadR1,             // R Bumper    | R       | R1       | Tweak Faster / Focus Next (in Windowing mode)
        GamepadL2,             // L Trigger   | ZL      | L2       | [Analog]
        GamepadR2,             // R Trigger   | ZR      | R2       | [Analog]
        GamepadL3,             // L Stick     | L3      | L3       |
        GamepadR3,             // R Stick     | R3      | R3       |
        GamepadLStickLeft,     //             |         |          | [Analog] Move Window (in Windowing mode)
        GamepadLStickRight,    //             |         |          | [Analog] Move Window (in Windowing mode)
        GamepadLStickUp,       //             |         |          | [Analog] Move Window (in Windowing mode)
        GamepadLStickDown,     //             |         |          | [Analog] Move Window (in Windowing mode)
        GamepadRStickLeft,     //             |         |          | [Analog]
        GamepadRStickRight,    //             |         |          | [Analog]
        GamepadRStickUp,       //             |         |          | [Analog]
        GamepadRStickDown,     //             |         |          | [Analog]

        // Mouse Buttons
        MouseLeft,
        MouseRight,
        MouseMiddle,
        MouseX1,
        MouseX2,
        MouseWheelX,
        MouseWheelY,
    };

    static Key StringToKey(const str& keyName)
    {
        // JavaScript code to convert inner enum content text
        // to the map values text starting from LeftArrow:
        // console.log(text.split('\n').map(e => e.split('//')[0].split(',')).flat().map(e => e.trim()).filter(e => e !== '').map(e => `{"${e}", Key::${e}},`).join(`\n`));
        // Next element added manually:
        // {"Tilde", Key::GraveAccent},
        static const std::map<std::string, Key> keyMap = {
            {"Tab", Key::Tab},
            {"LeftArrow", Key::LeftArrow},
            {"RightArrow", Key::RightArrow},
            {"UpArrow", Key::UpArrow},
            {"DownArrow", Key::DownArrow},
            {"PageUp", Key::PageUp},
            {"PageDown", Key::PageDown},
            {"Home", Key::Home},
            {"End", Key::End},
            {"Insert", Key::Insert},
            {"Delete", Key::Delete},
            {"Backspace", Key::Backspace},
            {"Space", Key::Space},
            {"Enter", Key::Enter},
            {"Escape", Key::Escape},
            {"LeftCtrl", Key::LeftCtrl},
            {"LeftShift", Key::LeftShift},
            {"LeftAlt", Key::LeftAlt},
            {"LeftSuper", Key::LeftSuper},
            {"RightCtrl", Key::RightCtrl},
            {"RightShift", Key::RightShift},
            {"RightAlt", Key::RightAlt},
            {"RightSuper", Key::RightSuper},
            {"Shift", Key::Shift},
            {"Ctrl", Key::Ctrl},
            {"Alt", Key::Alt},
            {"Super", Key::Super},
            {"Menu", Key::Menu},
            {"Num0", Key::Num0},
            {"Num1", Key::Num1},
            {"Num2", Key::Num2},
            {"Num3", Key::Num3},
            {"Num4", Key::Num4},
            {"Num5", Key::Num5},
            {"Num6", Key::Num6},
            {"Num7", Key::Num7},
            {"Num8", Key::Num8},
            {"Num9", Key::Num9},
            {"A", Key::A},
            {"B", Key::B},
            {"C", Key::C},
            {"D", Key::D},
            {"E", Key::E},
            {"F", Key::F},
            {"G", Key::G},
            {"H", Key::H},
            {"I", Key::I},
            {"J", Key::J},
            {"K", Key::K},
            {"L", Key::L},
            {"M", Key::M},
            {"N", Key::N},
            {"O", Key::O},
            {"P", Key::P},
            {"Q", Key::Q},
            {"R", Key::R},
            {"S", Key::S},
            {"T", Key::T},
            {"U", Key::U},
            {"V", Key::V},
            {"W", Key::W},
            {"X", Key::X},
            {"Y", Key::Y},
            {"Z", Key::Z},
            {"F1", Key::F1},
            {"F2", Key::F2},
            {"F3", Key::F3},
            {"F4", Key::F4},
            {"F5", Key::F5},
            {"F6", Key::F6},
            {"F7", Key::F7},
            {"F8", Key::F8},
            {"F9", Key::F9},
            {"F10", Key::F10},
            {"F11", Key::F11},
            {"F12", Key::F12},
            {"F13", Key::F13},
            {"F14", Key::F14},
            {"F15", Key::F15},
            {"F16", Key::F16},
            {"F17", Key::F17},
            {"F18", Key::F18},
            {"F19", Key::F19},
            {"F20", Key::F20},
            {"F21", Key::F21},
            {"F22", Key::F22},
            {"F23", Key::F23},
            {"F24", Key::F24},
            {"Apostrophe", Key::Apostrophe},
            {"Comma", Key::Comma},
            {"Minus", Key::Minus},
            {"Period", Key::Period},
            {"Slash", Key::Slash},
            {"Semicolon", Key::Semicolon},
            {"Equal", Key::Equal},
            {"LeftBracket", Key::LeftBracket},
            {"Backslash", Key::Backslash},
            {"RightBracket", Key::RightBracket},
            {"GraveAccent", Key::GraveAccent},
            {"Tilde", Key::GraveAccent},
            {"CapsLock", Key::CapsLock},
            {"ScrollLock", Key::ScrollLock},
            {"NumLock", Key::NumLock},
            {"PrintScreen", Key::PrintScreen},
            {"Pause", Key::Pause},
            {"Keypad0", Key::Keypad0},
            {"Keypad1", Key::Keypad1},
            {"Keypad2", Key::Keypad2},
            {"Keypad3", Key::Keypad3},
            {"Keypad4", Key::Keypad4},
            {"Keypad5", Key::Keypad5},
            {"Keypad6", Key::Keypad6},
            {"Keypad7", Key::Keypad7},
            {"Keypad8", Key::Keypad8},
            {"Keypad9", Key::Keypad9},
            {"KeypadDecimal", Key::KeypadDecimal},
            {"KeypadDivide", Key::KeypadDivide},
            {"KeypadMultiply", Key::KeypadMultiply},
            {"KeypadSubtract", Key::KeypadSubtract},
            {"KeypadAdd", Key::KeypadAdd},
            {"KeypadEnter", Key::KeypadEnter},
            {"KeypadEqual", Key::KeypadEqual},
            {"AppBack", Key::AppBack},
            {"AppForward", Key::AppForward},
            {"Oem102", Key::Oem102},
            {"GamepadStart", Key::GamepadStart},
            {"GamepadBack", Key::GamepadBack},
            {"GamepadFaceLeft", Key::GamepadFaceLeft},
            {"GamepadFaceRight", Key::GamepadFaceRight},
            {"GamepadFaceUp", Key::GamepadFaceUp},
            {"GamepadFaceDown", Key::GamepadFaceDown},
            {"GamepadDpadLeft", Key::GamepadDpadLeft},
            {"GamepadDpadRight", Key::GamepadDpadRight},
            {"GamepadDpadUp", Key::GamepadDpadUp},
            {"GamepadDpadDown", Key::GamepadDpadDown},
            {"GamepadL1", Key::GamepadL1},
            {"GamepadR1", Key::GamepadR1},
            {"GamepadL2", Key::GamepadL2},
            {"GamepadR2", Key::GamepadR2},
            {"GamepadL3", Key::GamepadL3},
            {"GamepadR3", Key::GamepadR3},
            {"GamepadLStickLeft", Key::GamepadLStickLeft},
            {"GamepadLStickRight", Key::GamepadLStickRight},
            {"GamepadLStickUp", Key::GamepadLStickUp},
            {"GamepadLStickDown", Key::GamepadLStickDown},
            {"GamepadRStickLeft", Key::GamepadRStickLeft},
            {"GamepadRStickRight", Key::GamepadRStickRight},
            {"GamepadRStickUp", Key::GamepadRStickUp},
            {"GamepadRStickDown", Key::GamepadRStickDown},
            {"MouseLeft", Key::MouseLeft},
            {"MouseRight", Key::MouseRight},
            {"MouseMiddle", Key::MouseMiddle},
            {"MouseX1", Key::MouseX1},
            {"MouseX2", Key::MouseX2},
            {"MouseWheelX", Key::MouseWheelX},
            {"MouseWheelY", Key::MouseWheelY},
        };

        if (auto it = keyMap.find(keyName); it != keyMap.end())
        {
            return it->second;
        }

        return Key::None;
    }
}
