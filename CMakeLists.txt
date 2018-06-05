cmake_minimum_required(VERSION 3.11)
project(deliberate_typing)

include(GNUInstallDirs)

find_package(Curses REQUIRED)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(typing src/main.cc)

target_link_libraries(typing curses stdc++fs)

target_compile_features(typing
    PRIVATE
        cxx_std_17
    )

target_compile_options(typing
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:Clang>:-Weverything>
    )

# YouCompleteMe setup
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
add_custom_target(link_for_compile_commands ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink
                  ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json)

install(TARGETS typing DESTINATION "${CMAKE_INSTALL_BINDIR}")
