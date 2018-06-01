cmake_minimum_required(VERSION 3.11)
project(deliberate_typing)

find_package(Curses REQUIRED)

add_executable(typing
    src/main.cc
    )

target_link_libraries(typing
    curses)

target_compile_features(typing
    PRIVATE
        cxx_std_17
    )

target_compile_options(typing
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:Clang>:-Weverything>
    )
