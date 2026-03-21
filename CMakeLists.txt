cmake_minimum_required(VERSION 3.14)
project(Turnstile VERSION 0.1.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the executable
add_executable(turnstile
    src/main.cpp
)

# Target include directories for the executable
target_include_directories(turnstile PRIVATE include)

# Optional: Enable warnings
if(MSVC)
    target_compile_options(turnstile PRIVATE /W4)
else()
    target_compile_options(turnstile PRIVATE -Wall -Wextra -pedantic)
endif()
