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

    # main
    ../injector-main.cpp

    ../injector-actuator.cpp

    ../../01_streamer/streamer-actuator.cpp
    ../../01_streamer/streamer-process.cpp
    ../../02_tokenizer/tokenizer-actuator.cpp
    ../../03_blocker/blocker-actuator.cpp
    ../../04_abstractor-init/abstractor-init-actuator.cpp
    ../../04_abstractor-init/details/abstractor-expression.cpp
    ../../04_abstractor-init/details/abstractor-stmt-expression.cpp
    ../../04_abstractor-init/details/abstractor-stmt-jmp.cpp
    ../../04_abstractor-init/details/abstractor-stmt-var-decl.cpp
    ../../05_pertainer/pertainer-actuator.cpp
)

func_add_common_target_link_libraries(arash)
