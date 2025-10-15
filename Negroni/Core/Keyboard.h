#pragma once

#include "Base.h"
#include "Logger.h"

#include <functional>
#include <unordered_map>

namespace Core
{
    typedef std::function<void()>           Action;
    typedef std::unordered_map<str, Action> KeyBindings;
    typedef std::function<void(u8)>         KeyListener;
    typedef std::function<void(const str&)> KeyListenerCallback;
    typedef std::unordered_map<str, str>    ActionKeyMap;
    typedef std::unordered_map<str, u8>     KeyCodeMap;
    typedef std::unordered_map<str, u8>     ActionCodeMap;

    struct KeyStatus
    {
        u8   code;
        bool isDown;
        bool isAltDown;
    };

    class Keyboard
    {
    public:
        static void Update(const KeyStatus& keyStatus)
        {
            if (keyStatus.isDown && Keyboard::keysInUse[keyStatus.code] == false) // Perform discrete action
            {
                Keyboard::keyHandler(keyStatus.code);
            }

            Keyboard::keysInUse[keyStatus.code] = keyStatus.isDown;
        }

        static void Listen(const str& actionName, const KeyListenerCallback& callback)
        {
            Keyboard::keyHandler = [actionName, &callback](u8 keyCode) {   // Switch to listen mode
                Keyboard::actionCodeMap[actionName] = keyCode;
                for (auto& it : Keyboard::keyCodeMap)
                {
                    if (it.second == keyCode)
                    {
                        callback(it.first);
                        break;
                    }
                }
                Keyboard::keyHandler = Keyboard::CallBindedAction;         // Back to binded mode
            };
        }

        static void CallBindedAction(u8 keyCode)
        {
            for (auto& it : Keyboard::actionCodeMap)
            {
                if (it.second == keyCode)
                {
                    if (Keyboard::keyBindings.contains(it.first))
                    {
                        Keyboard::keyBindings[it.first]();
                    }
                    break;
                }
            }
        }

        static void UseKeyBindings(const ActionKeyMap& actionKeyMap)
        {
            for (auto& it : actionKeyMap)
            {
                if (!Keyboard::keyCodeMap.contains(it.second))
                    continue;

                Keyboard::actionCodeMap[it.first] = Keyboard::keyCodeMap[it.second];
            }
        }

        static bool IsDown(const str& actionName)
        {
            if (!Keyboard::actionCodeMap.contains(actionName))
                return false;

            u8 keyCode = Keyboard::actionCodeMap[actionName];

            return Keyboard::keysInUse[keyCode];
        }

        static Action& OnPress(const str& actionName)
        {
            return Keyboard::keyBindings[actionName];
        }

        inline static bool Blocked = false;

    private:
        inline static KeyListener   keyHandler = Keyboard::CallBindedAction;
        inline static KeyBindings   keyBindings = {};
        inline static bool          keysInUse[256];
        inline static ActionCodeMap actionCodeMap = {};
        inline static Action        dummyAction = []() {};
        inline static KeyCodeMap    keyCodeMap = { // FIXME: This is very specific to Win32
            {"A", 65},
            {"D", 68},
            {"R", 82},
            {"S", 83},
            {"W", 87},
            {"J", 74},
            {"Space", 32},
            {"Escape", 27},
            {"Tilde", 192}
        };
    };
}
