cmake_minimum_required(VERSION 3.11)
project(markov_typing)

#set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
find_package(nlohmann_json QUIET)
find_package(CLI11 QUIET)

set(CLANG_FORMAT FALSE CACHE BOOL "Format the code with clang format.")
set(USE_BOOST_FS FALSE CACHE BOOL "Use boost filesystem. Does not work atm.")

if(USE_BOOST_FS)
    find_package(Boost REQUIRED COMPONENTS filesystem)
    set(FS_LINK Boost::filesystem)
    add_compile_options(-DBOOST_FILESYSTEM)
else()
    set(FS_LINK stdc++fs)
endif()

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
            https://github.com/CLIUtils/CLI11/releases/download/v1.6.1/CLI11.hpp
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

add_executable(markov-typing ${TYPING_SOURCES})

target_link_libraries(markov-typing 
    PRIVATE
        ${CURSES_LIBRARIES}
        # TODO it would be better to use boost::filesystem until compilers
        # stabilize filesystem support
        ${FS_LINK}
)

target_compile_features(markov-typing
    PRIVATE
        cxx_std_17
)

target_compile_options(markov-typing
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:Clang>:-Weverything>
        $<$<CONFIG:Debug>:-DDEBUG>
)

target_include_directories(markov-typing PRIVATE third_party)

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
install(TARGETS markov-typing DESTINATION "${CMAKE_INSTALL_BINDIR}")
