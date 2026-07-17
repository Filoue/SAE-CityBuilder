cmake_minimum_required(VERSION 3.20)
project(CityBuilder)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- FetchContent for libraries ---
include(FetchContent)

# NLOHMANN_JSON
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
# --- End FetchContent ---

add_subdirectory(core)
add_subdirectory(api)
add_subdirectory(game)

add_executable(main main.cc)

target_link_libraries(main PUBLIC core api game)
target_include_directories(main PUBLIC
    ${nlohmann_json_SOURCE_DIR}/include
)
