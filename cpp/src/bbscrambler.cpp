#include "bbscrambler.h"

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

extern "C" EXPORT void bbscrambler(const bool* u0, bool* y0)
{
    uint16_t prbs_register = 0x4A40;
    int frame_size = 6312;
    for (int i = 0; i < frame_size; i++) {

        int bit14 = (prbs_register >> 13) & 1;
        int bit15 = (prbs_register >> 14) & 1;
        int feedback = bit14 ^ bit15;

        int prbs_output = prbs_register & 1;

        int input_bit = u0[i] ? 1 : 0;
        int scrambled_bit = input_bit ^ prbs_output;

        y0[i] = scrambled_bit;

        prbs_register >>= 1;
        if (feedback) {
            prbs_register |= (1 << 14);
        }
    }
}