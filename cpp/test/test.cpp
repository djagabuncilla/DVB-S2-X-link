#include <iostream>
#include <windows.h>
#include <random>
#include <memory>
#include <cmath>
#include <iomanip>

// типы функций из DLL
typedef void (*bbscrambler_func)(int, const bool*, bool*);
typedef void (*plscrambler_func)(int, const double*, const double*, double*, double*);
typedef void (*pldescrambler_func)(int, const double*, const double*, double*, double*);

int main()
{
    const int Kbch = 6312;
    const int PL_FRAME = 8280;  // DFL для short frame, QPSK

    // --- загрузка DLL ---
    HMODULE lib = LoadLibraryA("dvbs2.dll");
    if (!lib) { std::cout << "Failed to load dvbs2.dll\n"; return -1; }

    // --- BB scrambler test (ваш существующий код) ---
    auto bbscrambler = (bbscrambler_func)GetProcAddress(lib, "bbscrambler");
    if (!bbscrambler) { std::cout << "Failed to find bbscrambler\n"; return -1; }

    std::unique_ptr<bool[]> u(new bool[Kbch]);
    std::unique_ptr<bool[]> y(new bool[Kbch]);
    std::unique_ptr<bool[]> z(new bool[Kbch]);

    std::mt19937 rng(12345);
    std::bernoulli_distribution bit(0.5);
    for (int i = 0; i < Kbch; i++) u[i] = bit(rng);

    bbscrambler(Kbch, u.get(), y.get());
    bbscrambler(Kbch, y.get(), z.get());

    int bb_errors = 0;
    for (int i = 0; i < Kbch; i++) if (u[i] != z[i]) bb_errors++;
    std::cout << "BB scrambler: " << (bb_errors == 0 ? "PASSED" : "FAILED (" + std::to_string(bb_errors) + " errors)") << "\n";

    auto print_bit_row = [&](const char* tag, const bool* data, int count = 10) {
        std::cout << tag;
        for (int k = 0; k < count; ++k)
             std::cout << std::setw(2) << (int)data[k];
             std::cout << "\n";
        };

    print_bit_row("IN : ", u.get(), 30);
    print_bit_row("SC : ", y.get(), 30);
    print_bit_row("DE : ", z.get(), 30);
    // --- PL scrambler/descrambler test ---
    auto plscramble = (plscrambler_func)GetProcAddress(lib, "plscrambler");
    auto pldescramble = (pldescrambler_func)GetProcAddress(lib, "pldescrambler");
    if (!plscramble || !pldescramble) { std::cout << "Failed to find PL functions\n"; return -1; }

    std::unique_ptr<double[]> re_in(new double[PL_FRAME]);
    std::unique_ptr<double[]> im_in(new double[PL_FRAME]);
    std::unique_ptr<double[]> re_scr(new double[PL_FRAME]);
    std::unique_ptr<double[]> im_scr(new double[PL_FRAME]);
    std::unique_ptr<double[]> re_out(new double[PL_FRAME]);
    std::unique_ptr<double[]> im_out(new double[PL_FRAME]);

    // генерируем тестовые комплексные отсчёты
    std::uniform_real_distribution<double> amp(-1.0, 1.0);
    for (int i = 0; i < PL_FRAME; i++) {
        re_in[i] = amp(rng);
        im_in[i] = amp(rng);
    }

    // scrambler --> descrambler
    plscramble(PL_FRAME, re_in.get(), im_in.get(), re_scr.get(), im_scr.get());
    pldescramble(PL_FRAME, re_scr.get(), im_scr.get(), re_out.get(), im_out.get());
    std::cout << std::fixed << std::setprecision(3);


    // проверка с допуском для float
    int pl_errors = 0;
    const double eps = 1e-10;
    for (int i = 0; i < PL_FRAME; i++) {
        if (std::abs(re_in[i] - re_out[i]) > eps || std::abs(im_in[i] - im_out[i]) > eps)
            pl_errors++;
    }
    std::cout << "PL scrambler: " << (pl_errors == 0 ? "PASSED" : "FAILED (" + std::to_string(pl_errors) + " errors)") << "\n";
    
    auto print_row = [&](const char* tag, const double* r, const double* i) {
        std::cout << tag;
        for (int k = 0; k < 4; ++k)
            std::cout << "[" << std::setw(6) << r[k] 
                     << (i[k] >= 0 ? " +" : " -") << std::abs(i[k]) << "i] ";
        std::cout << "\n";
    };

    print_row("IN : ", re_in.get(), im_in.get());
    print_row("SC : ", re_scr.get(), im_scr.get());
    print_row("DE : ", re_out.get(), im_out.get());
    
    FreeLibrary(lib);
    return 0;
}