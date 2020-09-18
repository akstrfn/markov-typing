cmake_minimum_required(VERSION 3.11)
project(markov_typing)

find_package(Qt5Widgets REQUIRED)
find_package(nlohmann_json QUIET)
find_package(CLI11 QUIET)

set(CLANG_FORMAT FALSE CACHE BOOL "Format the code with clang format.")
set(USE_BOOST_FS FALSE CACHE BOOL "Use boost filesystem. Does not work atm.")

set(CMAKE_AUTOMOC ON)
cmake_policy(SET CMP0100 NEW)

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
    ${CMAKE_SOURCE_DIR}/src/main.cc
    ${CMAKE_SOURCE_DIR}/src/probability_matrix.hh
    ${CMAKE_SOURCE_DIR}/src/probability_matrix.cc
    ${CMAKE_SOURCE_DIR}/src/sentence.hh
    ${CMAKE_SOURCE_DIR}/src/stats.hh
    ${CMAKE_SOURCE_DIR}/src/utils.hh
    ${CMAKE_SOURCE_DIR}/src/io.hh
    ${CMAKE_SOURCE_DIR}/src/io.cc
    ${CMAKE_SOURCE_DIR}/src/qt_gui/mainwindow.cc
    ${CMAKE_SOURCE_DIR}/src/qt_gui/practice_area.cc
    ${CMAKE_SOURCE_DIR}/src/qt_gui/char_sets_picker.cc
)

add_executable(markov-typing ${TYPING_SOURCES})

target_link_libraries(markov-typing 
    PRIVATE
        Qt5::Widgets
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
        $<$<CXX_COMPILER_ID:Clang>:-Wall>
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
