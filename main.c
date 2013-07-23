#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "neonblit.h"

//#define USE_MEMCPY
//#define USE_INLINE_ASSEMBLY

#define BYTES_PER_PIXEL 2

#define GREEN       (0x3f << 5)
#define RED         (0x1f << 11)
#define BLACK       (0x0)
#define WHITE       (0xffff)

#define COLOR_KEY   GREEN

#define WIDTH           800
#define HEIGHT          480
#define BUFFER_SIZE     (WIDTH * HEIGHT * BYTES_PER_PIXEL)

#ifdef USE_INLINE_ASSEMBLY
static inline void neon_blit(uint16_t *pDest, const uint16_t *pSrc, int numPixels, uint16_t chroma)
{
    asm volatile (
        "   vdup.16 q0, %3\n\t"
        "   vmov.i64 q15, #0xFFFFFFFFFFFFFFFF\n\t"
        "neonblit16loop:\n\t"
        "   vld4.16  {d8 - d11}, [%0]\n\t"
        "   vld4.16  {d4 - d7}, [%1]!\n\t"
        "   vceq.u16  q6, q0, q4\n\t"
        "   vceq.u16  q7, q0, q5\n\t"
        "   vbsl q6, q2, q4\n\t"
        "   vbsl q7, q3, q5\n\t"
        "   vst4.16  {d12 - d15}, [%0]!\n\t"
        "   subs %2, %2, #16\n\t"
        "   bgt neonblit16loop\n\t"
        : "=r"(pDest), "=r"(pSrc), "=r"(numPixels), "=r"(chroma)
        : "0"(pDest), "1"(pSrc), "2"(numPixels), "3"(chroma)
        : "memory", "q0", "q2", "q3", "q4", "q5", "q6", "q7", "q15"
    );
}
#endif

int main()
{
    int i, j, fd;

    uint16_t* pSrc = (uint16_t*)malloc(BUFFER_SIZE);
    uint16_t* pFramebuffer;
#ifdef USE_MEMCPY
    uint16_t* pDest = (uint16_t*)malloc(BUFFER_SIZE);
#else
    uint16_t* pDest;
#endif

    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0) {
        printf("Could not open framebuffer device\n");
        return 1;
    }

    pFramebuffer = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pFramebuffer == MAP_FAILED) {
        printf("Could not mmap framebuffer device\n");
        return 1;
    }

#ifndef USE_MEMCPY
    pDest = pFramebuffer;
#endif

    /* Create black / white halves */
    for (i = 0; i < HEIGHT; ++i)
    {
        for (j = 0; j < WIDTH; ++j)
        {
            if (j < WIDTH/2)
                pSrc[(i * WIDTH) + j] = WHITE;
            else
                pSrc[(i * WIDTH) + j] = BLACK;
        } 
    }

    /* Create green / red bars */
    for (i = 0; i < HEIGHT; ++i)
    {
        for (j = 0; j < WIDTH; ++j)
        {
            if ((i%32) < 16)
                pDest[(i * WIDTH) + j] = GREEN;
            else
                pDest[(i * WIDTH) + j] = RED;
        } 
    }

#ifdef USE_INLINE_ASSEMBLY
    neon_blit(pDest, pSrc, (WIDTH * HEIGHT), COLOR_KEY);
#else
    Neon_ChromaBlit16(pDest, pSrc, (WIDTH * HEIGHT), COLOR_KEY);
#endif

#ifdef USE_MEMCPY
    memcpy(pFramebuffer, pDest, BUFFER_SIZE);
#endif

    close(fd);

    free(pSrc);
#ifdef USE_MEMCPY
    free(pDest);
#endif

    return 0;
}
