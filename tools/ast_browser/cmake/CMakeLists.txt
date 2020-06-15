cmake_minimum_required(VERSION 2.8.9)

project (ast_browser)


include(${CMAKE_CURRENT_SOURCE_DIR}/../../../00_common/common_cmake.txt)

include_directories(../../../)


add_executable(ast_browser
    ../../../libs/easytext.cpp
    ../../../libs/filesystem.cpp
    ../../../libs/profiler.cpp

    ../../../00_common/serializer.cpp
    ../../../00_common/utils.cpp
    ../../../languages/language-en.cpp
    ../../tools-common/html/html.cpp

    # main
    ../ast_browser-main.cpp
)

func_add_common_target_link_libraries(ast_browser)
