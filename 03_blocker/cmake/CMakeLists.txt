cmake_minimum_required(VERSION 2.8.9)

project (arash)


include(${CMAKE_CURRENT_SOURCE_DIR}/../../00_common/common_cmake.txt)

include_directories(../../)


add_executable(arash
    ../../libs/easytext.cpp
    ../../libs/filesystem.cpp
    ../../libs/profiler.cpp

    ../../00_common/serializer.cpp
    ../../00_common/utils.cpp
    ../../00_common/helper.cpp
    ../../languages/language-en.cpp

    # main
    ../blocker-actuator.cpp
    ../blocker-main.cpp
)

func_add_common_target_link_libraries(arash)
