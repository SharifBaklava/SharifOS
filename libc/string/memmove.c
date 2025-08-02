#include <string.h>
#include <stdint.h>

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

// void * memmove(void *dst, const void *src, size_t len){
//     uint8_t *dp = (uint8_t *)dst;
//     const uint8_t *sp = (const uint8_t *)src;
    
//     if(sp < dp && sp + len > dp){
//         sp += len;
//         dp += len;
//         while(len-- > 0){
//             *--dp = *--sp;
//         }
//     }else{
//         while(len-- > 0){
//             *dp++ = *sp++;
//         }
//     }
    
//     return dst;
// }
