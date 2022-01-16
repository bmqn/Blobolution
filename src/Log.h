#pragma once

#include <ctime>
#include <iomanip>
#include <iostream>

class Log
{
public:
    template<typename...Args>
    static void logInfo(Args&& ...args)
    {
        std::time_t now = std::time(nullptr);

        std::cout << "[INFO](" << std::put_time(std::localtime(&now), "%T") << "): ";
        (std::cout << ... << std::forward<Args>(args));
        std::cout << '\n';
    }

    template<typename...Args>
    static void logWarn(Args&& ...args)
    {
        std::time_t now = std::time(nullptr);

        std::cout << "[WARN](" << std::put_time(std::localtime(&now), "%T") << "): ";
        (std::cout << ... << std::forward<Args>(args));
        std::cout << '\n';
    }

    template<typename...Args>
    static void logError(Args&& ...args)
    {
        std::time_t now = std::time(nullptr);

        std::cout << "[ERROR](" << std::put_time(std::localtime(&now), "%T") << "): ";
        (std::cout << ... << std::forward<Args>(args));
        std::cout << '\n';
    }
};

#define BL_LOG_INFO(...) Log::logInfo(__VA_ARGS__)
#define BL_LOG_WARN(...) Log::logWarn(__VA_ARGS__)
#define BL_LOG_ERROR(...) Log::logError(__VA_ARGS__)