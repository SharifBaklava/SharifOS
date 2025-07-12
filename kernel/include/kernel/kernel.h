#pragma once
#include <kernel/tty.h>
#include <kernel/memory/memorymanager.h>

#define krn Kernel::i
class Kernel
{
public:
    Terminal terminal;
    MemoryManager memoryManager;
    Kernel() = default;
    Kernel(const Kernel &) = delete;
    Kernel &operator=(const Kernel &) = delete;
    void initialize(uintptr_t p_multiboot_info);
    static Kernel i;
};
