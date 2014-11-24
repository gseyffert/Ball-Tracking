cmake_minimum_required(VERSION 2.8)
project( BlurImage )
find_package( OpenCV )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( HoughCircles HoughCircles.cpp )
target_link_libraries( HoughCircles ${OpenCV_LIBS} )
