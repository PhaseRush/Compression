project(compressionalg)
cmake_minimum_required(VERSION 3.13)

add_executable(compression main.cpp stats.cpp twoDtree.cpp cs221util/HSLAPixel.cpp cs221util/PNG.cpp cs221util/RGBAPixel.cpp cs221util/lodepng/lodepng.cpp)
target_include_directories(compression PUBLIC ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cs221util ${CMAKE_SOURCE_DIR}/cs221util/lodepng)
# this is a comment i know this much ty woah