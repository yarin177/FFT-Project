cmake_minimum_required(VERSION 3.1.0)  # CMake version check
project(NeuralNetwork)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES NeuralNetwork.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(main main.cpp ${SOURCE_FILES})

find_package(iir)
target_link_libraries(main iir::iir_static)
