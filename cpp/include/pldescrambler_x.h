#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void pldescrambler_x(int frame_size, const double *u_re, const double *u_im, double *y_re, double *y_im);

#ifdef __cplusplus
}
#endif