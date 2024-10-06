cmake_minimum_required(VERSION 3.30)

project(dmd2gltf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm CONFIG REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

set(DMD_ROUTES_DIR ${PROJECT_SOURCE_DIR}/routes/dmd)
set(GLTF_ROUTES_DIR ${PROJECT_SOURCE_DIR}/routes/gltf)

configure_file(cmake_defines.h.in cmake_defines.h)

file(GLOB SOURCES src/*.cpp)

add_executable(dmd2gltf ${SOURCES})

target_link_libraries(dmd2gltf PRIVATE
    glm::glm
    ImageMagick::Magick++
)

target_include_directories(dmd2gltf PRIVATE
    ${PROJECT_BINARY_DIR}
    include
)
