cmake_minimum_required(VERSION 2.8)
project( BallTracking )
find_package( OpenCV )

#find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif()

include_directories( ${OpenCV_INCLUDE_DIRS} )
#add_executable( DetectBallCV DetectBallCV.cpp )
#target_link_libraries( DetectBallCV ${OpenCV_LIBS} )

add_executable( main main.cpp DetectBallCV.cpp graph/composeGraph.cpp)
target_link_libraries( main ${OpenCV_LIBS} )
