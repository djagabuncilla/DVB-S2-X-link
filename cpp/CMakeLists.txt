cmake_minimum_required(VERSION 3.10)

project(DVBS2_LINK)

set(CMAKE_CXX_STANDARD 11)

# подключаем заголовки
include_directories(include)

# библиотека (твой код)
add_library(dvbs2 SHARED
    src/bbscrambler.cpp
    src/pilot_insertion.cpp
    src/plscrambler.cpp
    src/pldescrambler.cpp
    src/phasecorrection.cpp
    src/pldescrambler_x.cpp
)

#Критично для Windows/MinGW: убираем префикс "lib", чтобы на выходе был dvbs2.dll
set_target_properties(dvbs2 PROPERTIES PREFIX "")

#Если в .h файле используете макрос #ifdef DVBS2_EXPORTS, раскомментируйте:
#target_compile_definitions(dvbs2 PRIVATE DVBS2_EXPORTS)