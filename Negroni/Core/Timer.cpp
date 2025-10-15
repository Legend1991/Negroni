#include "Timer.h"

namespace Core
{
    class Timer::Task
    {
    public:
        uint64_t priority;
        Action callback;
        millisec delay;
        millisec expiration;
        uint32_t times;

        bool waiting(millisec now) const
        {
            return expiration > now;
        }

        void execute() const
        {
            callback();
        }

        bool after(Task_ptr otherTask) const
        {
            return laterThan(otherTask) || higherPriorityThan(otherTask);
        }

        bool higherPriorityThan(Task_ptr otherTask) const
        {
            return expiration == otherTask->expiration && priority > otherTask->priority;
        }

        bool laterThan(Task_ptr otherTask) const
        {
            return expiration > otherTask->expiration;
        }

        bool reschedule(millisec now)
        {
            --times;
            if (times <= 0) return false;
            expiration = now + delay;
            return true;
        }
    };

    bool Timer::set(Action callback, millisec delay, uint32_t times, millisec now)
    {
        if (times < 1) return false;
        auto task = std::make_shared<Task>(count, callback, delay, now + delay, times);
        add(task);
        ++count;
        return true;
    }

    void Timer::tick(millisec now)
    {
        for (auto task : schedule)
        {
            if (task->waiting(now)) break;
            task->execute();
            schedule.pop_front();
            if (task->reschedule(now)) add(task);
        }
    }

    void Timer::add(Task_ptr newTask)
    {
        for (auto taskIt = schedule.begin(); taskIt != schedule.end(); ++taskIt)
        {
            auto& task = *taskIt;
            if (task->after(newTask))
            {
                schedule.insert(taskIt, newTask);
                return;
            }
        }
        schedule.push_back(newTask);
    }

    bool Timer::empty()
    {
        return schedule.size() == 0;
    }
}
