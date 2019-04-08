
cmake_minimum_required (VERSION 2.6)
project (CMAKETEST C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)
find_library(libgtest, REQUIRED)

include(cmake/clang-dev-tools.cmake)

enable_testing()

add_executable(d1 d1/d1.cpp)
add_executable(d2 d2/d2.cpp)
add_executable(d3 d3/d3.cpp)
add_executable(d4 d4/d4.cpp)
add_executable(d5 d5/d5.cpp)

add_executable(d6 d6/d6.cpp)
add_executable(d6t d6/td6.cpp)
target_link_libraries(d6t gtest gtest_main)
target_link_libraries(d6t ${CMAKE_THREAD_LIBS_INIT} )
add_test(test_d6 d6t)

add_executable(d7 d7/d7.cpp)
add_executable(d7t d7/td7.cpp)
target_link_libraries(d7t gtest gtest_main)
target_link_libraries(d7t ${CMAKE_THREAD_LIBS_INIT} )
add_test(test_d7 d7t)

add_executable(d8 d8/d8.cpp)
add_executable(d9 d9/d9.cpp)
add_executable(d10 d10/d10.cpp)
add_executable(d11 d11/d11.cpp)
add_executable(d12 d12/d12.cpp)
add_executable(d13 d13/d13.cpp)
add_executable(d14 d14/d14.cpp)
add_executable(d15 d15/d15.cpp)
add_executable(d15t d15/td15.cpp)
target_link_libraries(d15t gtest gtest_main)
target_link_libraries(d15t ${CMAKE_THREAD_LIBS_INIT} )
add_test(test_d15 d15t)

add_executable(d16 d16/d16.cpp)
add_executable(d17 d17/d17.cpp)
add_executable(d18 d18/d18.cpp)
add_executable(d19 d19/d19.cpp)
add_executable(d20 d20/d20.cpp)
