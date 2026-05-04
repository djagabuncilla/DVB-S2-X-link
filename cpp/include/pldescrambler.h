#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void pldescrambler(int frame_size_without_PLS, const double *u_re, const double *u_im, double *y_re, double *y_im);

#ifdef __cplusplus
}
#endif