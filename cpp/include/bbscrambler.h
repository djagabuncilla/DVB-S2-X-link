#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef DVBS2_EXPORTS
        #define DVBS2_API __declspec(dllexport)
    #else
        #define DVBS2_API __declspec(dllimport)
    #endif
#else
    #define DVBS2_API
#endif

DVBS2_API void bbscrambler(const bool* u0, bool* y0);

#ifdef __cplusplus
}
#endif