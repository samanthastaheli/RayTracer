cmake_minimum_required(VERSION 3.20)
project(RayTracer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(RayTracer "main.cc" "vec3.h"  "color.h" "ray.h" )