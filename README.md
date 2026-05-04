# DVB-S2-link
## Структура проекта
```text
├── StartupConf.mlx
├── README.md
├── .gitignore
│
├── Simulink Models/
│   ├── dvbs2transmiter.slx
│   ├── dvbs2link_a.slx
│   └── dvbs2link.slx
│
├── Matlab code/
│   └── CMakeBuild.m
│
├── Matlab Blocks/
│   ├── PLS_Codec.m
│   ├── Pi2BPSKModulator.m
│   ├── generate_dvbs2_frames.m
│   ├── GenBCHPoly.m
│   └── FramesSynchronizer.m
│
├── MAT files/
│   ├── SOF.mat
│   ├── LDPC_2_5.mat
│   └── dvbs2LDPCParityMatrix...
│
├── cpp/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── plscrambler.cpp
│   │   ├── pldescrambler.cpp
│   │   ├── pilot_insertion.cpp
│   │   ├── phasecorrection.cpp
│   │   └── bbscrambler.cpp
│   │
│   └── include/
│       ├── plscrambler.h
│       ├── pldescrambler.h
│       ├── pilot_insertion.h
│       ├── phasecorrection.h
│       └── bbscrambler.h
│
└── .git/
```
### Применение
Для начала нужно выполнить файл ```StartupConf.mlx```. Он подгрузит данные,
матрицы LDPC, SOF последовательность, соберёт .dll для C++ функций, 
откроет модель. Далее в модели dvbs2link.slx нужно запустить симуляцию. 
Если модель не запускается из-за 
несовпадения версий, раскоментируйте следующие строчки в файле ```StartupConf.mlx```: 
```matlab
%modelfile = "Simulink Models\dvbs2link_a.slx"; 
%modelname = "dvbs2link_a";
```
### Особенности
Используется схема линии для QPSK 2/5 для short frame.
Декодирование PLS не предусмотрено, так как нам и так известен modcod. 
Синхронизация при низком соотношении сигнал/шум (примерно 6 dB) очень 
прохая (пока выясняю причины), как и детектирование PL-фрейма.
(этот блок вообще придётся с нуля писать). 


# Описание функций
## Matlab Blocks
#### PLS_Codec.m

Генерация 64-битного PLS кода преамбулы. Сейчас она не нужна, так как 
реализацию создания сигнала я убрал.

#### Pi2BPSKModulator.m

Реализация pi/2-BPSK модулятора на основе стандартного BPSK-модулятора.
Написан QWENом, так как нужен был чисто для получения SOF.

#### generate_dvbs2_frames.m

Генерирует сигнал реального DVB-S2 на основе ```comm.dvbs2WaveformGenerator```.
Требует ```dvbs2xLDPCParityMatrices.mat``` для получения матриц LDPC. 

Параметры сигнала задаются в коде:
```matlab
gen = dvbs2WaveformGenerator( ...
        'StreamFormat','TS', ... % GS или TS
        'NumInputStreams',1, ... % количество потоков
        'FECFrame','short', ...  % параметры FECFRame
        'MODCOD',3, ...          % 3 - соответствует QPSK 2/5
        'HasPilots',true, ...    % наличие пилотов
        'DFL', 6232, ...         % Размер поля данных Kbch - 80
        'SamplesPerSymbol',4);   % SPS
```
Использование:
```matlab
[WaveIn, BitsIn] = generate_dvbs2_frames(1000);
%Вход - количество генерируемых PLFrame
%Выход: WaveTn - отчёты сгенерированного сигнала
%       BitsIn - сгенерированный поток данных длиной 6016, по которому
%       и создаётся сигнал (нужен для вычисления ber).
```
#### FrameSynchronizer.m

Стандартный блок синхронизации кадров (поиска преамбулы), который
поставлялся в одном из официальных примеров Simulink. Работает плохо
при низком SNR, не соотвествует стандарту. Пока как заглушка, позже 
реализую нормальную версию по спецификации DVB-S2X.

#### GenBCHPoly.m

Код вычисления полинома для BCH-декодера. Входной параметр - число t - 
количество исправляемых ошибок, то есть - количество перемножаемых полиномов. 
```matlab
    g = GenBCHPoly(num_poly); % num_poly = t - количество 
                          % перемножаемых полиномов
```

#### CMakeBuild.m

Функция для сборки всех ```cpp``` и ```h``` в ```dvbs2.dll```. Требуется ```CMake``` и ```MinGW64``` в
```PATH```. Билд создаётся в папке ```cpp/build```. 

## C++ Blocks
Находятся в папке ```/cpp```. Для ```cpp``` - ```/cpp/src```, для ```h``` - ```/cpp/include```.

#### pldescrambler

Дескремблер физического уровня. Создаётся последовательность Гольда
по спецификации DVB-S2, по которой уже и делается поворот фазы каждого символа.
Входной сигнал - скремблированный PL-frame без PLS, выходной - дескремблированный.
```cpp
void pldescrambler(int frame_size_without_PLS, // размер PLFrame без SOF
                       const double *u_re,     // синфазная составляющая входного сигнала
                       const double *u_im,     // квадратурная составляющая входного сигнала
                       double *y_re,           // то же самое для выходного сигнала
                       double *y_im)
```
#### bbscrambler

Скремблер базовой полосы. Соответственно, работает как дескремблер. 
```cpp
void bbscrambler(int Kbch,       // Длина bbframe до кодирования
                 const bool* u0, // входной вектор
                 bool* y0)       // выходной вектор
```

#### phasecorrection

Синхронизация фазы по пилотам. Ищет пилоты по позициям в кадре, вычисляет фазу,
строит линейную аппроксимацию, затем корректирует фазу. Также удаляет пилоты.
По-факту - такая же заглушка, так как реализацию под DVB-S2X ещё предстоит сделать.
```cpp
void phase_correction(const double *u_re, 
                      const double *u_im, // входной PL-Frame
                      double *y_re,
                      double *y_im)       // выходной PL-frame без пилотов
```

#### plscrambler
PL-скремблер. Требовался в реализации создания сигнала. На данный момент 
не используется.

#### pilot_insertion
Вставка в PL-frame пилот-последовательностей. На данный момент не используется.

# Особенности работы модели
Параметры канала задаются ползунками в левой нижней части холста модели. Заголовок
физического уровня PLHEADER удаляется с помощью demux, также как и BBHEADER. Проверка
работы модели проиводится путём сравнения входного кадра и выходного после полного тракта 
приёма без учёта CRC-8.  
