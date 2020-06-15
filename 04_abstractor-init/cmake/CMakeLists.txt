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
    ../../00_common/debug.cpp
    ../../languages/language-en.cpp

    ../details/abstractor-expression.cpp
    ../details/abstractor-stmt-expression.cpp
    ../details/abstractor-stmt-jmp.cpp
    ../details/abstractor-stmt-var-decl.cpp

    # main
    ../abstractor-init-actuator.cpp
    ../abstractor-init-main.cpp
)

func_add_common_target_link_libraries(arash)
