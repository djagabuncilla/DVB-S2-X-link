#include "pldescrambler_x.h"
#include <stdint.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// ============================================================
// DVB-S2X Physical Layer DESCRAMBLER
//
// Реализация соответствует MATLAB:
//
//   seq = 2*z2 + z1
//
// где:
//
//   z1 = x1 ^ y1
//   z2 = x2 ^ y2
//
// x2/y2 сдвинуты на 131072
//
// ------------------------------------------------------------
// IMPORTANT:
//
// Это DESCRAMBLER
// => используется conjugate rotation
//
// ------------------------------------------------------------
// Input:
//
//   весь PLFRAME целиком
//
// ------------------------------------------------------------
// Особенности:
//
//   - первые 90 symbols (PLHEADER) НЕ descramble
//   - VL-SNR header 900 symbols НЕ descramble
//   - scrambling sequence при этом продолжает идти
//
// ============================================================

static inline void step_x(uint32_t* reg)
{
    uint8_t b0 = (*reg) & 1;
    uint8_t b7 = ((*reg) >> 7) & 1;

    uint8_t fb = b0 ^ b7;

    *reg = (*reg >> 1) | ((uint32_t)fb << 17);
}

static inline void step_y(uint32_t* reg)
{
    uint8_t b0  = (*reg) & 1;
    uint8_t b5  = ((*reg) >> 5) & 1;
    uint8_t b7  = ((*reg) >> 7) & 1;
    uint8_t b10 = ((*reg) >> 10) & 1;

    uint8_t fb = b0 ^ b5 ^ b7 ^ b10;

    *reg = (*reg >> 1) | ((uint32_t)fb << 17);
}

extern "C" EXPORT void pldescrambler_x(
    int frame_size,
    const double* u_re,
    const double* u_im,
    double* y_re,
    double* y_im)
{
    // ========================================================
    // Main Gold sequence
    // ========================================================

    uint32_t reg_x1 = 0x00001;
    uint32_t reg_y1 = 0x3FFFF;

    // ========================================================
    // Shifted Gold sequence (+131072)
    // ========================================================

    uint32_t reg_x2 = reg_x1;
    uint32_t reg_y2 = reg_y1;

    for (int i = 0; i < 131072; i++)
    {
        step_x(&reg_x2);
        step_y(&reg_y2);
    }

    // ========================================================
    // Constants
    // ========================================================

    const int PLHEADER_SIZE = 90;

    // VL-SNR header:
    // immediately after PLHEADER
    const int VLSNR_HEADER_START = 90;
    const int VLSNR_HEADER_SIZE  = 900;
    const int VLSNR_HEADER_END =
        VLSNR_HEADER_START + VLSNR_HEADER_SIZE;

    // ========================================================
    // Entire PLFRAME
    // ========================================================

    for (int i = 0; i < frame_size; i++)
    {
        // ----------------------------------------------------
        // Generate scrambling integer:
        //
        // R = 2*z2 + z1
        // ----------------------------------------------------

        uint8_t z1 =
            ((reg_x1 & 1) ^ (reg_y1 & 1));

        uint8_t z2 =
            ((reg_x2 & 1) ^ (reg_y2 & 1));

        uint8_t R = (z2 << 1) | z1;

        // ----------------------------------------------------
        // Advance BOTH sequences
        // ----------------------------------------------------

        step_x(&reg_x1);
        step_y(&reg_y1);

        step_x(&reg_x2);
        step_y(&reg_y2);

        // ----------------------------------------------------
        // Input symbol
        // ----------------------------------------------------

        double I = u_re[i];
        double Q = u_im[i];

        // ----------------------------------------------------
        // Regions WITHOUT scrambling
        //
        // IMPORTANT:
        // sequence still advances
        // ----------------------------------------------------

        bool apply_descrambling = true;

        // PLHEADER
        if (i < PLHEADER_SIZE)
            apply_descrambling = false;

        // VL-SNR header
        if (i >= VLSNR_HEADER_START &&
            i <  VLSNR_HEADER_END)
        {
            apply_descrambling = false;
        }

        // ----------------------------------------------------
        // No descrambling
        // ----------------------------------------------------

        if (!apply_descrambling)
        {
            y_re[i] = I;
            y_im[i] = Q;
            continue;
        }

        // ----------------------------------------------------
        // DVB-S2/S2X DESCRAMBLER
        //
        // conjugate rotation
        //
        // exp(-j*pi/2 * R)
        // ----------------------------------------------------

        switch (R)
        {
            case 0:

                y_re[i] =  I;
                y_im[i] =  Q;

                break;

            case 1:

                y_re[i] =  Q;
                y_im[i] = -I;

                break;

            case 2:

                y_re[i] = -I;
                y_im[i] = -Q;

                break;

            case 3:

                y_re[i] = -Q;
                y_im[i] =  I;

                break;
        }
    }
}