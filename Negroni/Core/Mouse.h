#pragma once

#include "Base.h"
#include "Logger.h"

#include <unordered_map>

namespace Core
{
    typedef std::unordered_map<str, str> ActionMouseEventMap;
    typedef std::unordered_map<str, u32> ActionMouseCodeMap;
    typedef std::unordered_map<str, u32> MouseEventCodeMap;

    struct MouseStatus
    {
        i32  x;
        i32  y;
        i32  event;
        r32  delta; // Mouse wheel
    };

    class Mouse
    {
    public:
        static void Update(const MouseStatus& mouseStatus)
        {
            currentMouseStatus = mouseStatus;
        }

        static bool IsDown(const str& actionName)
        {
            if (!Mouse::actionCodeMap.contains(actionName))
                return false;

            bool res = currentMouseStatus.event == Mouse::actionCodeMap[actionName];
            currentMouseStatus.event = 0;
            return res;
        }

        static u32 X()
        {
            return currentMouseStatus.x;
        }

        static u32 Y()
        {
            return currentMouseStatus.y;
        }

        static void UseKeyBindings(const ActionMouseEventMap& actionMouseEventMap)
        {
            for (auto& it : actionMouseEventMap)
            {
                if (!Mouse::eventCodeMap.contains(it.second))
                    continue;

                Mouse::actionCodeMap[it.first] = Mouse::eventCodeMap[it.second];
            }
        }

        inline static bool Blocked = false;

    private:
        inline static MouseStatus currentMouseStatus;
        inline static ActionMouseCodeMap actionCodeMap = {};
        inline static MouseEventCodeMap eventCodeMap = { // FIXME: This is very specific to Win32
            {"Mouse_Move",        512},
            {"Mouse_LButtonDown", 513},
            {"Mouse_LButtonUp",   514},
        };
    };
}
