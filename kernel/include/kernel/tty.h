#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <utils/Singleton.h>
#include <stdint.h>
#include <string.h>
class Terminal
{
	const size_t VGA_WIDTH = 80;
	const size_t VGA_HEIGHT = 25;
	uint16_t *const VGA_MEMORY = (uint16_t *)0xB8000;

	size_t row;
	size_t column;
	uint8_t color;
	uint16_t *buffer;

public:
	void init(void);
	void putchar(char c);
	void write(const char *data, size_t size);
	void writestring(const char *data);
	void setcolor(uint8_t color);
	void putentryat(unsigned char c, uint8_t color, size_t x, size_t y);
	void scroll(size_t lines = 1);
	void cleanLine(size_t line);
		
};

#endif
