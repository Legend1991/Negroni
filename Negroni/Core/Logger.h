#pragma once

#include "Queue.h"
#include <iostream>
#include <format>
#include <functional>

#ifdef _MSC_VER // Microsoft compilers
    #define GET_ARG_COUNT(...)  INTERNAL_EXPAND_ARGS_PRIVATE(INTERNAL_ARGS_AUGMENTER(__VA_ARGS__))
    #define INTERNAL_ARGS_AUGMENTER(...) unused, __VA_ARGS__
    #define INTERNAL_EXPAND(x) x
    #define INTERNAL_EXPAND_ARGS_PRIVATE(...) INTERNAL_EXPAND(INTERNAL_GET_ARG_COUNT_PRIVATE(__VA_ARGS__, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
    #define INTERNAL_GET_ARG_COUNT_PRIVATE(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count
#else // Non-Microsoft compilers
    #define GET_ARG_COUNT(...) INTERNAL_GET_ARG_COUNT_PRIVATE(0, ## __VA_ARGS__, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
    #define INTERNAL_GET_ARG_COUNT_PRIVATE(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count
#endif

namespace Core
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Multithread logger part
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    extern struct LoggerQItem;

    extern Queue<LoggerQItem> loggerQueue;
    extern std::thread loggerThread;

    extern void loggerFunc(Queue<LoggerQItem>& in);
    extern void log(std::string message);
    extern void startLogger();
    extern void stopLogger();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    constexpr std::string logLocation(const std::string& prettyFunction)
    {
        size_t colons = prettyFunction.find("(");
        if (colons == std::string::npos) return "";
        // size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1; // Including namespace
        //size_t begin = prettyFunction.find("::") + 2; // Excluding namespace
        size_t begin = prettyFunction.rfind("::", prettyFunction.rfind("::", colons) - 1) + 2; // Excluding namespace. Fixed.
        size_t end = colons - begin;

        return prettyFunction.substr(begin, end);
    }

#if defined(_MSC_VER)
    #define __PRETTY_FUNCTION__     __FUNCSIG__
#endif

#define __LOG_LOCATION__    logLocation(__PRETTY_FUNCTION__)

#define CONSOLE_COLOR_RESET   "\u001b[0m"
#define CONSOLE_COLOR_RED     "\x1b[31m"
#define CONSOLE_COLOR_GREEN   "\x1b[32m"
#define CONSOLE_COLOR_YELLOW  "\x1b[33m"
#define CONSOLE_COLOR_BLUE    "\x1b[34m"
#define CONSOLE_COLOR_MAGENTA "\x1b[35m"
#define CONSOLE_COLOR_CYAN    "\x1b[36m"

#define LOG_LEVEL_INFO  std::format("{}Info{}",  CONSOLE_COLOR_CYAN, CONSOLE_COLOR_RESET)
#define LOG_LEVEL_WARN  std::format("{}Warn{}",  CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_RESET)
#define LOG_LEVEL_ERROR std::format("{}Error{}", CONSOLE_COLOR_RED,  CONSOLE_COLOR_RESET)

#define LOG(logLevel, ...) std::cout << std::format("[{}] {} {}", __LOG_LOCATION__, logLevel, __VA_ARGS__) << std::endl;
//#define LOG(logLevel, ...)  log(std::format("{} [{}] {}",  logLevel, __LOG_LOCATION__, __VA_ARGS__));

#define LOG_INFO(...)  LOG(LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOG_WARN(...)  LOG(LOG_LEVEL_WARN,  __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)

#define log_info(...)  LOG_INFO(std::format(__VA_ARGS__))
#define log_warn(...)  LOG_WARN(std::format(__VA_ARGS__))
#define log_error(...) LOG_ERROR(std::format(__VA_ARGS__))

#define log_size(...) LOG_INFO(GET_ARG_COUNT(__VA_ARGS__))

#if defined(DEBUG)
    #define LOG_LEVEL_DEBUG std::format("{}Debug:{}", CONSOLE_COLOR_MAGENTA, CONSOLE_COLOR_RESET)
    #define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)

    #define log_debug(...) LOG_DEBUG(std::format(__VA_ARGS__))
#else
    #define LOG_DEBUG(...)
    #define log_debug(...)
#endif
}
