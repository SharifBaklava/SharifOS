#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#include <kernel/kernel.h>

#endif
extern "C" {
int putchar(int ic) {
#if defined(__is_libk)
	char c = (char) ic;
	Kernel::i.terminal.write(&c, sizeof(c));
#else
	// TODO: Implement stdio and the write system call.
#endif
	return ic;
}
}