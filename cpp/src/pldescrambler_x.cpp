#include "pldescrambler_x.h"
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

extern "C" EXPORT void pldescrambler_x(
    int frame_size,
    int VL_SNR,
    int PLScrambler_seq,
    const double *u_re,
    const double *u_im,
    double *y_re,
    double *y_im)
{
    uint32_t reg_x, reg_y;
    uint32_t reg_x_shift, reg_y_shift;

    // Gold code index:
    // n = PLScrambler_seq * 10949
    uint32_t n = (uint32_t)(PLScrambler_seq * 10949);

    // Initial conditions
    reg_x = 1;
    reg_y = 0x3FFFF;

    reg_x_shift = reg_x;
    reg_y_shift = reg_y;

    // --------------------------------------------------------
    // IMPORTANT FIX:
    // n is applied only to x-sequence
    // y-sequence is NOT shifted by n
    // --------------------------------------------------------

    for (uint32_t i = 0; i < n; i++) {
        step_x(&reg_x);
    }

    for (uint32_t i = 0; i < n + 131072; i++) {
        step_x(&reg_x_shift);
    }

    for (uint32_t i = 0; i < 131072; i++) {
        step_y(&reg_y_shift);
    }

    // For ordinary DVB-S2/S2X:
    // scrambling starts after 90 symbols
    // For VL-SNR:
    // scrambling is not applied during the 900-symbol header,
    // but the sequence must still be advanced there
    int scrambling_start = (VL_SNR ? 990 : 90);

    for (int i = 0; i < scrambling_start; i++) {
        y_re[i] = u_re[i];
        y_im[i] = u_im[i];
    }

    for (int i = scrambling_start; i < frame_size; i++) {

        uint8_t x  = reg_x & 1;
        uint8_t y  = reg_y & 1;
        uint8_t x_s = reg_x_shift & 1;
        uint8_t y_s = reg_y_shift & 1;

        uint8_t z0 = x ^ y;
        uint8_t z1 = x_s ^ y_s;
        uint8_t R = 2*z1 + z0;

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