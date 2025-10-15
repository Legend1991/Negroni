#pragma once

#include "Clock.h"

#include <functional>
#include <deque>

namespace Core
{
    class Timer
    {
    public:
        typedef std::function<void()> Action;
        typedef Clock::millisec millisec;

        static bool set(Action callback, millisec delay, uint32_t times = 1, millisec now = Clock::now());
        static void tick(millisec now = Clock::now());
        static bool empty();

    private:
        typedef class Task;
        typedef std::shared_ptr<Task> Task_ptr;

        static inline std::deque<Task_ptr> schedule;
        static inline uint64_t count = 0;

        static void add(Task_ptr newTask);
    };
}
