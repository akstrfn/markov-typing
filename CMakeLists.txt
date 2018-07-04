cmake_minimum_required(VERSION 3.11)
project(deliberate_typing)

find_package(Curses REQUIRED)
find_package(nlohmann_json)
find_package(CLI11) #QUIET)

set(CLANG_FORMAT true CACHE BOOL "format the code with clang format")

if(NOT nlohmann_json_FOUND AND NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/nlohmann/json.hpp)
    message("Downloading nlohmann/json")
    file(DOWNLOAD
            https://github.com/nlohmann/json/releases/download/v3.1.2/json.hpp
            ${CMAKE_SOURCE_DIR}/third_party/nlohmann/json.hpp
        )
endif()

if(NOT CLI11_FOUND AND NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/CLI/CLI.hpp)
    message("Downloading CLI11")
    file(DOWNLOAD
            https://github.com/CLIUtils/CLI11/releases/download/v1.6.0/CLI11.hpp
            ${CMAKE_SOURCE_DIR}/third_party/CLI/CLI.hpp
        )
endif()

set(TYPING_SOURCES
    ${CMAKE_SOURCE_DIR}/src/curses_wrap.cc
    ${CMAKE_SOURCE_DIR}/src/curses_wrap.hh
    ${CMAKE_SOURCE_DIR}/src/main.cc
    ${CMAKE_SOURCE_DIR}/src/probability_matrix.hh
    ${CMAKE_SOURCE_DIR}/src/probability_matrix.cc
    ${CMAKE_SOURCE_DIR}/src/sentence.hh
    ${CMAKE_SOURCE_DIR}/src/stats.hh
    ${CMAKE_SOURCE_DIR}/src/utils.hh
    ${CMAKE_SOURCE_DIR}/src/io.hh
    ${CMAKE_SOURCE_DIR}/src/io.cc
)

add_executable(typing ${TYPING_SOURCES})

target_link_libraries(typing 
    PRIVATE
        ${CURSES_LIBRARIES}
        # TODO it would be better to use boost::filesystem until compilers
        # stabilize filesystem support
        stdc++fs
)

target_compile_features(typing
    PRIVATE
        cxx_std_17
)

target_compile_options(typing
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:Clang>:-Weverything>
        $<$<CONFIG:Debug>:-DDEBUG>
)

target_include_directories(typing PRIVATE third_party)

if(UNIX)
    # YouCompleteMe setup
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
    add_custom_target(link_for_compile_commands ALL
        COMMAND
            ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/compile_commands.json
    )

endif()

if(UNIX AND ${CLANG_FORMAT})
    add_custom_target(clang-format ALL
        clang-format -i ${TYPING_SOURCES}
    )
endif()

include(GNUInstallDirs)
install(TARGETS typing DESTINATION "${CMAKE_INSTALL_BINDIR}")
