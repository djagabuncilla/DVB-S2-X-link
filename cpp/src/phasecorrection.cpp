#include "phasecorrection.h"
#include <math.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT void phase_correction(const double *u_re, const double *u_im, double *y_re, double *y_im)
{
    const int SLOT_SIZE = 90;
    const int PILOT_BLOCK = 36;
    const int PILOT_PERIOD = 16;
    const int NUM_GROUPS = 5;
    const double PILOT_RE = 0.70710678;
    const double PILOT_IM = 0.70710678;

    double theta[NUM_GROUPS];
    int in_idx = 0;

    for (int g = 0; g < NUM_GROUPS; g++) {
        in_idx += SLOT_SIZE * PILOT_PERIOD;
        double sum_I = 0;
        double sum_Q = 0;
        for (int k = 0; k < PILOT_BLOCK; k++) {
            double I = u_re[in_idx];
            double Q = u_im[in_idx];
            double z_re = I * PILOT_RE + Q * PILOT_IM;
            double z_im = Q * PILOT_RE - I * PILOT_IM;
            sum_I += z_re;
            sum_Q += z_im;
            in_idx++;
        }
        theta[g] = atan2(sum_Q, sum_I);
    }

    for (int i = 1; i < NUM_GROUPS; i++) {
        while (theta[i] - theta[i-1] >  M_PI) theta[i] -= 2*M_PI;
        while (theta[i] - theta[i-1] < -M_PI) theta[i] += 2*M_PI;
    }

    double sum_i = 0, sum_phi = 0, sum_i2 = 0, sum_i_phi = 0;
    for (int i = 0; i < NUM_GROUPS; i++) {
        sum_i += i;
        sum_phi += theta[i];
        sum_i2 += i * i;
        sum_i_phi += i * theta[i];
    }

    double denom = NUM_GROUPS * sum_i2 - sum_i * sum_i;
    double a = (NUM_GROUPS * sum_i_phi - sum_i * sum_phi) / denom;
    double b = (sum_phi - a * sum_i) / NUM_GROUPS;
    double symbols_per_group = SLOT_SIZE * PILOT_PERIOD;
    a = a / symbols_per_group;

    in_idx = 0;
    int out_idx = 0;
    int data_symbol_counter = 0;

    for (int g = 0; g < NUM_GROUPS; g++) {
        for (int s = 0; s < SLOT_SIZE * PILOT_PERIOD; s++) {
            double I = u_re[in_idx];
            double Q = u_im[in_idx];
            double theta_k = a * data_symbol_counter + b;
            double c = cos(theta_k);
            double s_ = sin(theta_k);
            y_re[out_idx] =  I * c + Q * s_;
            y_im[out_idx] = -I * s_ + Q * c;
            in_idx++;
            out_idx++;
            data_symbol_counter++;
        }
        in_idx += PILOT_BLOCK;
    }

    for (int s = 0; s < 10 * SLOT_SIZE; s++) {
        double I = u_re[in_idx];
        double Q = u_im[in_idx];
        double theta_k = a * data_symbol_counter + b;
        double c = cos(theta_k);
        double s_ = sin(theta_k);
        y_re[out_idx] =  I * c + Q * s_;
        y_im[out_idx] = -I * s_ + Q * c;
        in_idx++;
        out_idx++;
        data_symbol_counter++;
    }
}