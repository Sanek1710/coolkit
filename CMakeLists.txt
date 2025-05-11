cmake_minimum_required(VERSION 3.10)
project(coolkit VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# Create an interface library
add_library(coolkit INTERFACE)

# Add include directories
target_include_directories(coolkit INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard
target_compile_features(coolkit INTERFACE cxx_std_17)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CMAKE_CXX_STANDARD 17)

  add_subdirectory(third-party)

  add_executable(main main.cpp)
  target_link_libraries(main coolkit)

  add_executable(fmt-main fmt-main.cpp)
  target_link_libraries(fmt-main coolkit fmt::fmt)
endif ()
