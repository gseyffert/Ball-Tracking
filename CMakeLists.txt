cmake_minimum_required(VERSION 2.8)
project( BallTracking )
find_package( OpenCV )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++ -g")

#find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif()

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( DetectBallCV test-DetectBallCV.cpp )
target_link_libraries( DetectBallCV ${OpenCV_LIBS} )

add_executable( Visualize test-Visualize.cpp )
target_link_libraries( Visualize ${OpenCV_LIBS} )

add_executable( main main.cpp DetectBallCV.cpp graph/composeGraph.cpp graph/freeGraph.cpp graph/shortestPath.cpp Visualize.cpp)
target_link_libraries( main ${OpenCV_LIBS} )

#add_executable( test-graph graph/composeGraph.cpp graph/test-composeGraph.cpp )
