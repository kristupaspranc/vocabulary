add_library(gui STATIC
    GUI.cpp
)

target_include_directories(gui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

find_library(ncurses ncurses REQUIRED)

target_link_libraries(gui PUBLIC
    database
    vocabulary_compiler_flags
)

target_link_libraries(gui PRIVATE
    ncurses
)
