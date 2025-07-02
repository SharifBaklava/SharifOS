#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

void Terminal::initialize(void)
{
	row = 0;
	column = 0;
	color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			buffer[index] = vga_entry(' ', color);
		}
	}
}

void Terminal::setcolor(uint8_t color)
{
	color = color;
}

void Terminal::putentryat(unsigned char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	buffer[index] = vga_entry(c, color);
}

void Terminal::putchar(char c)
{
	unsigned char uc = c;
	if (c == '\n')
	{
		column = 0;
		if (++row == VGA_HEIGHT)
		{
			// row = 0;
			scroll(1); // Scroll up one line if we reach the end of the screen

		}
	}
	else
	{
		putentryat(uc, color, column, row);
		if (++column == VGA_WIDTH)
		{
			column = 0;
			if (++row == VGA_HEIGHT)
				// row = 0;
				// writestring("ddddd\n");
				scroll(1); // Scroll up one line if we reach the end of the screen
		}
	}
}

void Terminal::write(const char *data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		putchar(data[i]);
}

void Terminal::writestring(const char *data)
{
	write(data, strlen(data));
}

void Terminal::scroll(int lines)
{
	char *dst = (char *)buffer;
	char *src = (char *)buffer + lines * VGA_WIDTH * 2;
	size_t size = (VGA_HEIGHT - lines) * VGA_WIDTH * 2;
	memmove(dst, src, size);
	row -= lines;
	column = 0;
	for (size_t i = VGA_HEIGHT - lines; i < VGA_HEIGHT; ++i)
	{
		for (size_t j = 0; j < VGA_WIDTH; ++j)
		{
			putentryat(' ', color, j, i);
		}
	}
}
// Terminal Terminal::terminal; // Static instance of Terminal