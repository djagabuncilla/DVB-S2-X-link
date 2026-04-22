#include "pldescrambler.h"
#include <stdint.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

static inline void step_x(uint32_t *reg)
{
    uint8_t x = (*reg) & 1;
    uint8_t fb = x ^ ((*reg >> 7) & 1);
    *reg = (*reg >> 1) | (fb << 17);
}

static inline void step_y(uint32_t *reg)
{
    uint8_t y = (*reg) & 1;
    uint8_t fb = y ^ ((*reg >> 5) & 1) ^ ((*reg >> 7) & 1) ^ ((*reg >> 10) & 1);
    *reg = (*reg >> 1) | (fb << 17);
}

extern "C" EXPORT void pldescrambler(const double *u_re, const double *u_im, double *y_re, double *y_im)
{
    static uint32_t reg_x, reg_y;
    static uint32_t reg_x_shift, reg_y_shift;
    const int FRAME_SIZE = 8280;

    reg_x = 1;
    reg_y = 0x1FFFF;
    reg_x_shift = reg_x;
    reg_y_shift = reg_y;

    for (int i = 0; i < 131072; i++) {
        step_x(&reg_x_shift);
        step_y(&reg_y_shift);
    }

    for (int i = 0; i < FRAME_SIZE; i++) {
        uint8_t x  = reg_x & 1;
        uint8_t y  = reg_y & 1;
        uint8_t x_s = reg_x_shift & 1;
        uint8_t y_s = reg_y_shift & 1;

        uint8_t z0 = x ^ y;
        uint8_t z1 = x_s ^ y_s;
        uint8_t R = (z1 << 1) | z0;

        double I = u_re[i];
        double Q = u_im[i];

        switch (R) {
            case 0: y_re[i] =  I; y_im[i] =  Q; break;
            case 1: y_re[i] =  Q; y_im[i] = -I; break;
            case 2: y_re[i] = -I; y_im[i] = -Q; break;
            case 3: y_re[i] = -Q; y_im[i] =  I; break;
        }

        step_x(&reg_x);
        step_y(&reg_y);
        step_x(&reg_x_shift);
        step_y(&reg_y_shift);
    }
}