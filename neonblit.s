.global Neon_ChromaBlit16

#******************************************************************************
#*
#*  Function:  Neon_ChromaBlit16
#*
#*  Args:      dest, src, width*height, chroma
#*              r0,   r1,   r2,            r3
#*
#*  Return:
#*
#*  Modified Registers:
#*              r0, r1 -- auto incremented during loop
#*              Q0    = chroma key
#*              Q2,Q3 = src data
#*              Q4,Q5 = dest data
#*              Q6,Q7 = chroma mask
#*              Q15 = ALL ONES
#*
#*  Purpose:   Performs a fast blit of dest and src based on chroma 
#*
#*  Notes:
#*
#******************************************************************************

Neon_ChromaBlit16:
                .text
                .fpu    neon

                 # Use an even number of registers to keep 64-bit alignment
                 push {r0, lr}

                 # Copy the chroma key into the vector
                 vdup.16 q0, r3

                 # Set q15 to all 1's
                 vmov.i64 q15, #0xFFFFFFFFFFFFFFFF

neonblit16loop:
#   Q0      Q1      Q2      Q3      Q4      Q5        Q6         Q7
# d0 d1   d2 d3   d4 d5   d6 d7   d8 d9   d10 d11   d12 d13   d14 d15
# Q0    = chroma key
# Q2,Q3 = src data
# Q4,Q5 = dest data
# Q6,Q7 = chroma mask
# Q15 = ALL ONES

                 vld4.16  {d8 - d11}, [r0]  @ Load dest into q4-q5
                 vld4.16  {d4 - d7}, [r1]!  @ Load source into q2-q3, then increment

                 # Populate Q6 and Q7 with all 1s where chroma is found
                 # all 0s if not
                 vceq.i16  q6, q0, q4
                 vceq.i16  q7, q0, q5

                 # Select pixels based on chroma mask
                 vbsl q6, q2, q4
                 vbsl q7, q3, q5

                 # Store result into destination buffer
                 vst4.16  {d12 - d15}, [r0]!

                 # Decrement num pixels and loop
                 subs r2, r2, #16
                 bgt neonblit16loop

                 pop {r0, pc}

                 bx lr


