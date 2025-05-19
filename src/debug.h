// First, create a debug utility header file: debug.h
#pragma once

#include <iostream>

class Debug
{
private:
    static bool enabled;

public:
    static void setEnabled(bool enable)
    {
        enabled = enable;
    }

    static bool isEnabled()
    {
        return enabled;
    }

    template <typename T>
    static void log(const T &message)
    {
        if (enabled)
        {
            std::cout << message << std::endl;
        }
    }

    template <typename T, typename... Args>
    static void log(const T &first, const Args &...args)
    {
        if (enabled)
        {
            std::cout << first;
            log(args...);
        }
    }
};

// Define the static member (this would go in a debug.cpp file)
bool Debug::enabled = false;

// Macro for easier debug logging
#define DEBUG_LOG(...) Debug::log(__VA_ARGS__)
#define DEBUG_ENABLED() Debug::isEnabled()
