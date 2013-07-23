#ifndef neonblit_h
#define neonblit_h

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Blit dest and source buffers
 * @param[in] pDest - buffer (framebuffer with chroma key)
 * @param[in] pSrc - buffer (3d data)
 * @param[in] numPixels - number of pixels (must be a multiple of 8)
 * @param[in] chromaKey - 16-bit chroma value
 */
extern void Neon_ChromaBlit16(uint16_t* pDest, uint16_t* pSrc, int numPixels, uint16_t chromaKey);

#ifdef __cplusplus
};
#endif
#endif
