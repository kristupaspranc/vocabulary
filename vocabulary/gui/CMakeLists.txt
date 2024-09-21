add_library(gui STATIC
    src/GUI.cpp
)

target_include_directories(gui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_library(ncurses ncurses REQUIRED)

target_link_libraries(gui PUBLIC
    ncurses
    database
)
