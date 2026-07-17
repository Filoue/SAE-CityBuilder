cmake_minimum_required(VERSION 3.20)
project(CityBuilder)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- FetchContent for libraries ---
include(FetchContent)



# --- End FetchContent ---

add_subdirectory(core)
add_subdirectory(api)
add_subdirectory(game)

add_executable(main main.cc)

target_link_libraries(main PUBLIC core api game)

