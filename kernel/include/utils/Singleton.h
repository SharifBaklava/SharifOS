#pragma once
template <typename T>
class Singleton
{
public:
    // Delete copy constructor and assignment operator to prevent copies
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // Provides global access to the singleton instance
    static T& Instance()
    {
        static T instance; // Guaranteed to be created once, thread-safe in C++11+
        return instance;
    }

protected:
    Singleton() = default;  // Allow construction only by derived classes
    ~Singleton() = default;
};

