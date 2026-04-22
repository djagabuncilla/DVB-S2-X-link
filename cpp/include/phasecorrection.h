#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void phase_correction(const double *u_re, const double *u_im, double *y_re, double *y_im);

#ifdef __cplusplus
}
#endif