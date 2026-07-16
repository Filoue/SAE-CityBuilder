find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cc)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_library(game ${SRC_FILES} src/resource_manager.cc ${HEADER_FILES} include/resource_manager.h)

target_include_directories(game PUBLIC include/)

target_link_libraries(game PUBLIC api)
target_link_libraries(game PUBLIC SFML::System SFML::Graphics SFML::Window)
