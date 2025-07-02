#pragma once
#include <kernel/tty.h>

class Kernel {
public:
    Terminal terminal;

    Kernel() =default;
    Kernel(const Kernel&) = delete;
    Kernel& operator=(const Kernel&) = delete;

    static Kernel i;
};

