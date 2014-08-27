project(finch)

cmake_minimum_required(VERSION 2.8.12)

find_package(CUDA REQUIRED)
set(CUDA_NVCC_FLAGS "-arch=sm_30")

if(APPLE)
  set(CUDA_HOST_COMPILER "/usr/local/bin/g++")
  set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")
endif()

add_definitions(-std=c++1y)
set(CUDA_NVCC_FLAGS -std=c++11)

option(debug "Enable debugging flags" OFF)

if(debug)
  add_definitions(-g)
endif()

option(gui "Build with GUI tools" OFF)

include_directories("${CMAKE_SOURCE_DIR}/include")
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*")
file(GLOB CUDA_SOURCES "${CMAKE_SOURCE_DIR}/src/cuda/*")
file(GLOB MAZE_GEN_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_gen/*")
file(GLOB MAZE_RUN_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_run/*")

add_executable("${CMAKE_PROJECT_NAME}-maze_gen" ${MAZE_GEN_SOURCES})
cuda_add_executable("${CMAKE_PROJECT_NAME}" ${SOURCES} ${CUDA_SOURCES})

if(gui)
  find_package(Qt5Widgets REQUIRED)
  file(GLOB UI_SOURCES "${CMAKE_SOURCE_DIR}/src/ui/*")
  file(GLOB MAZE_VIEW_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_view/*")
  
  add_library("${CMAKE_PROJECT_NAME}-ui" SHARED ${UI_SOURCES})
  qt5_use_modules("${CMAKE_PROJECT_NAME}-ui" Widgets)
  add_executable("${CMAKE_PROJECT_NAME}-maze_view" ${MAZE_VIEW_SOURCES})
  target_link_libraries("${CMAKE_PROJECT_NAME}" "${CMAKE_PROJECT_NAME}-ui")
  target_link_libraries("${CMAKE_PROJECT_NAME}-maze_view" "${CMAKE_PROJECT_NAME}-ui")
endif()

