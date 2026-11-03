cmake_minimum_required(VERSION 3.16)
project(lab2_elf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab2_elf
    src/main.cpp
    src/ElfParser.cpp
    src/Scanner.cpp
    src/Reporter.cpp
)

# Указываем компилятору, где искать заголовочные файлы
target_include_directories(lab2_elf PRIVATE include)