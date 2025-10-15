#pragma once

#include "Base.h"
#include "Logger.h"
#include "Key.h"

// Usefull sources:
// https://docs.unity3d.com/6000.2/Documentation/Manual/class-InputManager.html
// https://docs.unity3d.com/6000.2/Documentation/ScriptReference/Input.html

namespace Core
{
    constexpr auto KEYS_IN_USE_LENGTH = 256;

    struct InputEvent
    {
        i32 mousePositionX = 0;
        i32 mousePositionY = 0;
        i32 mousePositionDeltaX = 0;
        i32 mousePositionDeltaY = 0;
        r32 mouseScrollDelta = 0;

        bool keysInUse[KEYS_IN_USE_LENGTH];

        // To know if there were any user input done
        bool isDirty = false;

        InputEvent()
        {
            memset(this, 0, sizeof(InputEvent));
        }
    };

    typedef std::function<void()>        Action;
    typedef std::unordered_map<str, str> ActionKeyMap;
    typedef std::unordered_map<str, u8>  ActionCodeMap;

    class Input
    {
    public:
        static void Update(InputEvent& newInput)
        {
            if (newInput.isDirty)
            {
                if (newInput.mousePositionX == 0 && newInput.mousePositionY == 0)
                {
                    newInput.mousePositionX = input.mousePositionX;
                    newInput.mousePositionY = input.mousePositionY;
                }

                newInput.mousePositionDeltaX = newInput.mousePositionX - input.mousePositionX;
                newInput.mousePositionDeltaY = newInput.mousePositionY - input.mousePositionY;

                input = newInput;
            }
            else
            {
                input.mousePositionDeltaX = 0;
                input.mousePositionDeltaY = 0;
                input.mouseScrollDelta = 0;
                input.isDirty = false;
            }

            //if (newInput.isDirty)
            //{
            //    log_info("InputEvent size {}", sizeof(InputEvent));
            //    log_info("X\t{}", input.mousePositionX);
            //    log_info("Y\t{}", input.mousePositionY);
            //    log_info("DeltaX\t{}", input.mousePositionDeltaX);
            //    log_info("DeltaY\t{}", input.mousePositionDeltaY);
            //    log_info("ScrollDelta  {}", input.mouseScrollDelta);
            //    for (int i = 0; i < KEYS_IN_USE_LENGTH; ++i)
            //    {
            //        if (input.keysInUse[i])
            //        {
            //            log_info("keysInUse[{}] {}", i, input.keysInUse[i]);
            //        }
            //    }
            //    log_info("isDirty  {}\n", input.isDirty);
            //}
        }

        static void UseKeyBindings(const ActionKeyMap& actionKeyMap)
        {
            for (const auto& it : actionKeyMap)
            {
                Key key = StringToKey(it.second);
                if (key == Key::None)
                {
                    log_warn("Unknown key/button name: \"{}\"", it.second);
                    continue;
                }

                actionCodeMap[it.first] = key;
            }
        }

        static bool IsDown(const str& actionName)
        {
            if (!actionCodeMap.contains(actionName))
                return false;

            u8 keyCode = actionCodeMap[actionName];

            return input.keysInUse[keyCode];
        }

        inline static bool isKeyboardBlocked = false;
        inline static bool isMouseBlocked = false;

    private:
        inline static ActionCodeMap actionCodeMap = {};

        inline static InputEvent input;
    };
}
