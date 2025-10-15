#pragma once

#include "Base.h"

#include <cassert>
#include <chrono>
#include <string>

using namespace std::chrono;

#ifdef PROFILER_ENABLED
    #define ProfileBlock(name) Profiler _(name);
    #define ProfileClear() Profiler::size = 0;
#else
    #define ProfileBlock(name)
    #define ProfileClear()
#endif

namespace Core
{
    constexpr u32 PROFILER_MAX_ENTRIES = 1024;

    class Profiler
    {
    public:
        struct Entry
        {
            str name;
            u32 indent = 0;
            i64 elapsed = 0;
        };

        inline static Entry entries[PROFILER_MAX_ENTRIES];
        inline static i32 size = 0;

        Profiler(const std::string& name)
        {
            assert(Profiler::size < PROFILER_MAX_ENTRIES);
            this->index = Profiler::size;

            auto& entry = Profiler::entries[this->index];
            entry.name = name;
            entry.indent = Profiler::indent;

            Profiler::size = (Profiler::size + 1) % PROFILER_MAX_ENTRIES;
            Profiler::indent += 1;

            this->start = high_resolution_clock::now();
        }

        ~Profiler()
        {
            auto end = high_resolution_clock::now();

            Profiler::indent -= 1;

            auto& entry = Profiler::entries[this->index];
            entry.elapsed = duration_cast<microseconds>(end - start).count();
        }

        static void Clear()
        {
            Profiler::size = 0;
        }

    private:
        u32 index;
        steady_clock::time_point start;

        inline static u32 indent = 0;
    };
}
