#include "pilot_insertion.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT void pilot_insertion(const double *u_re, const double *u_im, double *y_re, double *y_im)
{
    const int SLOT_SIZE = 90;
    const int FRAME_SIZE = 8100;
    const int PILOT_BLOCK = 36;
    const int PILOT_PERIOD = 16;
    const double PILOT_RE = 0.70710678;
    const double PILOT_IM = 0.70710678;
    int in_idx = 0;
    int out_idx = 0;
    int slot_count = 0;
    while (in_idx < FRAME_SIZE) {
        for (int i = 0; i < SLOT_SIZE; i++) {
            y_re[out_idx] = u_re[in_idx];
            y_im[out_idx] = u_im[in_idx];
            out_idx++;
            in_idx++;
        }
        slot_count++;
        if (slot_count % PILOT_PERIOD == 0) {
            for (int k = 0; k < PILOT_BLOCK; k++) {
                y_re[out_idx] = PILOT_RE;
                y_im[out_idx] = PILOT_IM;
                out_idx++;
            }
        }
    }
}