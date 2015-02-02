project(finch)

cmake_minimum_required(VERSION 2.8.12)

set(UI_DIR ${CMAKE_SOURCE_DIR}/ui)

option(cuda "Use CUDA" OFF)

if(cuda)
  find_package(CUDA REQUIRED)
  add_definitions(-DUSE_CUDA)
endif()
set(CUDA_NVCC_FLAGS "-arch=sm_30")

add_definitions(-std=c++1y)
set(CUDA_NVCC_FLAGS -std=c++11)

option(debug "Enable debugging flags" OFF)

if(debug)
  add_definitions(-g)
endif()

add_definitions(-pthread)

option(gui "Build with GUI tools" OFF)

include_directories("${CMAKE_SOURCE_DIR}/include")
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*")
file(GLOB NORMAL_SOURCES "${CMAKE_SOURCE_DIR}/src/normal/*")
file(GLOB CUDA_SOURCES "${CMAKE_SOURCE_DIR}/src/cuda/*")
file(GLOB CPU_SOURCES "${CMAKE_SOURCE_DIR}/src/cpu/*")
file(GLOB MAZE_GEN_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_gen/*")
file(GLOB MAZE_RUN_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_run/*")
file(GLOB MAZE_DIGEST_GEN_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_digest_gen/*")

add_executable("${CMAKE_PROJECT_NAME}-maze_gen" ${MAZE_GEN_SOURCES} ${CMAKE_SOURCE_DIR}/src/jsoncpp.cpp)
add_executable("${CMAKE_PROJECT_NAME}-maze_digest_gen" ${MAZE_DIGEST_GEN_SOURCES})
if(cuda)
  cuda_add_executable("${CMAKE_PROJECT_NAME}" ${SOURCES} ${CUDA_SOURCES} ${CPU_SOURCES})
else()
  add_executable("${CMAKE_PROJECT_NAME}" ${SOURCES} ${CPU_SOURCES} ${NORMAL_SOURCES})
endif()

if(gui)
  find_package(Qt5Widgets REQUIRED)
  file(GLOB UI_SOURCES "${CMAKE_SOURCE_DIR}/src/ui/*")
  file(GLOB MAZE_RUN_UIS "${UI_DIR}/agent_stepper.ui")
  file(GLOB GRADIENT_VIEW_UIS "${UI_DIR}/gradient_view.ui")
  file(GLOB MAZE_VIEW_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_view/*")
  file(GLOB GRADIENT_VIEW_SOURCES "${CMAKE_SOURCE_DIR}/src/gradient_view/*")
  file(GLOB MAZE_RUN_SOURCES "${CMAKE_SOURCE_DIR}/src/maze_run/*")
  qt5_wrap_ui(MAZE_RUN_UI_SOURCES ${MAZE_RUN_UIS})
  file(GLOB MAZE_RUN_UI_INCLUDES ${CMAKE_SOURCE_DIR}/include/finch/maze_run/*.hpp)
  qt5_wrap_cpp(MAZE_RUN_UI_SOURCES ${MAZE_RUN_UI_INCLUDES})
  qt5_wrap_ui(GRADIENT_VIEW_UI_SOURCES ${GRADIENT_VIEW_UIS})
  file(GLOB GRADIENT_VIEW_UI_INCLUDES ${CMAKE_SOURCE_DIR}/include/finch/gradient_view/*.hpp)
  qt5_wrap_cpp(GRADIENT_VIEW_UI_SOURCES ${GRADIENT_VIEW_UI_INCLUDES})
  include_directories(${CMAKE_BINARY_DIR})
  add_library("${CMAKE_PROJECT_NAME}-ui" SHARED ${UI_SOURCES})
  
  qt5_use_modules("${CMAKE_PROJECT_NAME}-ui" Widgets)
  add_executable("${CMAKE_PROJECT_NAME}-maze_view" ${MAZE_VIEW_SOURCES})
  add_executable("${CMAKE_PROJECT_NAME}-gradient_view" ${GRADIENT_VIEW_SOURCES} ${GRADIENT_VIEW_UI_SOURCES})
  add_executable("${CMAKE_PROJECT_NAME}-maze_run" ${MAZE_RUN_SOURCES} ${MAZE_RUN_UI_SOURCES} ${CPU_SOURCES} ${SOURCES})
  qt5_use_modules("${CMAKE_PROJECT_NAME}-maze_run" Widgets)
  qt5_use_modules("${CMAKE_PROJECT_NAME}-gradient_view" Widgets)
  target_link_libraries("${CMAKE_PROJECT_NAME}" "${CMAKE_PROJECT_NAME}-ui")
  target_link_libraries("${CMAKE_PROJECT_NAME}-maze_view" "${CMAKE_PROJECT_NAME}-ui")
  target_link_libraries("${CMAKE_PROJECT_NAME}-gradient_view" "${CMAKE_PROJECT_NAME}-ui")
  target_link_libraries("${CMAKE_PROJECT_NAME}-maze_run" "${CMAKE_PROJECT_NAME}-ui")
endif()

