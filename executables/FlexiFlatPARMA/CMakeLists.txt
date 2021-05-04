cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(parma_exec)

#begining of the new stuff:
if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS "-std=c++11 -O3")
message(STATUS "**********************************Build type: ${CMAKE_BUILD_TYPE}")
#end of the new stuff

find_package(PCL 1.2 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")


add_executable (parma_exec main.cc ../../PARMA_flat/FlexiFlatPARMA.cc ../../PARMA_flat/PARMA.cc ../../globals/globals.cc ../../core_lib/idxXYZ.cc ../../clustering_lib/clusterer.cc ../../core_lib/ParmaConcurrentMap.cc ../../core_lib/map.cc ../../core_lib/ParmaConcurrentEllipsoid.cc ../../core_lib/Ellipsoid.cc ../../core_lib/delimitingBox.cc)


target_link_libraries (parma_exec ${PCL_LIBRARIES} gsl gslcblas gomp tbb)
