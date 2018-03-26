cmake_minimum_required(VERSION 3.9)
project(seraph C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURC_DIR}/bin")

add_executable(${PROJECT_NAME} src/main.c)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
)
