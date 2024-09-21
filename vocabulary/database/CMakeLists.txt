add_library(database STATIC
    src/DatabaseCreation.cpp
    src/DatabaseUtils.cpp
    src/DatabaseTools.cpp
)

target_include_directories(database PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_package(SQLite3 REQUIRED)
target_link_libraries(database PRIVATE sqlite3)
