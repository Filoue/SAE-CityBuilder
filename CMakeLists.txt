cmake_minimum_required(VERSION 4.0.1)
project(CityBuilder)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_subdirectory(core/)
add_subdirectory(api/)
add_subdirectory(game/)

#custom target
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
        COMMENT "🔄 Copying all resources to build directory"
)

#executable
add_executable(CityBuilder main.cpp)

#dependencies
add_dependencies(CityBuilder copy_resources)

target_link_libraries(CityBuilder PRIVATE game)
