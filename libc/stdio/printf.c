#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int itoa(int value, char* buffer, int base) {
	int i = 0;
	do {
		buffer[i++] = "0123456789abcdef"[value % base];
	} while ((value /= base) != 0);
	buffer[i] = '\0';
	reverse(buffer, i);
	return i;
}
void reverse(char* buffer, int length) {
	int start = 0;
	int end = length - 1;
	while (start < end) {
		char temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
		start++;
		end--;
	}
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);
	
	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
		}
		else if(*format == 'd') {
			format++;
			int digit = va_arg(parameters, int);
			char buffer[12];
			int digit_size = itoa(digit, buffer, 10);
			 if (buffer == NULL) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			size_t len = strlen(buffer);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(buffer, len))
				return -1;
			written += len;
		
		}
		else if(*format == 'x') {
			format++;
			int digit = va_arg(parameters, int);
			char buffer[12];
			int digit_size = itoa(digit, buffer, 16);
			 if (buffer == NULL) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			size_t len = strlen(buffer);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(buffer, len))
				return -1;
			written += len;
		
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
