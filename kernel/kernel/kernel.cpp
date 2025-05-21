#include <stdio.h>


extern "C"
{
	#include <kernel/tty.h>
} 

class kernel
{
private:
	/* data */
public:
	kernel()
	{
		terminal_initialize();

		printf("Hello, kernel Worldsssssssss!\n");
	}
};

kernel k;
extern "C" void kernel_main(void) {
	// terminal_initialize();
	printf("Hello, kernel World!\n");
}
