#include "bbscrambler.h"
#include <stdint.h>

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

extern "C" EXPORT void bbscrambler(int Kbch, const bool* u0, bool* y0)
{
    uint16_t prbs_register = 0x4A80;   // reset на каждый BBFRAME

    for (int i = 0; i < Kbch; i++)
    {
        int prbs = (prbs_register ^ (prbs_register >> 1)) & 1;

        y0[i] = u0[i] ^ prbs;

        prbs_register >>= 1;
        if (prbs)
        {
            prbs_register |= 0x4000;
        }
    }
}