#include <kernel/gdt.h>

void set_gdt_entry(GDTEntry* entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    entry->limit_low   = limit & 0xFFFF;
    entry->base_low    = base & 0xFFFF;
    entry->base_mid    = (base >> 16) & 0xFF;
    entry->access      = access;
    entry->granularity = ((gran & 0xF0) | ((limit >> 16) & 0x0F));
    entry->base_high   = (base >> 24) & 0xFF;
}
