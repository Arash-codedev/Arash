cmake_minimum_required(VERSION 2.8.9)

project (arash)


include(${CMAKE_CURRENT_SOURCE_DIR}/../../../00_common/common_cmake.txt)

include_directories(../../../)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


add_executable(arash
    ../../../libs/easytext.cpp
    ../../../libs/filesystem.cpp
    ../../../libs/profiler.cpp

    ../../../00_common/serializer.cpp
    ../../../00_common/utils.cpp
    ../../../languages/language-en.cpp

    # main
    ../codegen-actuator.cpp
    ../codegen-main.cpp
)

func_add_common_target_link_libraries(arash)

target_link_libraries(arash LLVM)
