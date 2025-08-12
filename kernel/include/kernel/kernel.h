#pragma once
#include <kernel/tty.h>
#include <kernel/memory/memorymanager.h>
#include <kernel/gdt.h>
#include <kernel/interrupts/InterruptManager.h>

#define krn Kernel::i
class Kernel
{
public:
    Terminal terminal;
    MemoryManager memoryManager;
    GDT gdtInitializer;
    InterruptManager interruptManager;
    Kernel() = default;
    Kernel(const Kernel &) = delete;
    Kernel &operator=(const Kernel &) = delete;
    void init(uintptr_t p_multiboot_info);
    static Kernel i;
};
