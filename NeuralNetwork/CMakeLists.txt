cmake_minimum_required (VERSION 3.8)
project (main)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_executable(main main.cpp NeuralNetwork.cpp)
