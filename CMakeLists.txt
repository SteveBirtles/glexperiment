cmake_minimum_required(VERSION 3.10)
project(MyOpenGLProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

set(GLAD_INCLUDE_DIR libraries/glad/)
set(GLAD_SOURCE libraries/glad/glad.c)

if(APPLE)
    include_directories(
        ${OPENGL_INCLUDE_DIR}
        /opt/homebrew/include
        ${GLAD_INCLUDE_DIR}
    )
    link_directories(/opt/homebrew/lib)
elseif(UNIX AND NOT APPLE)
    include_directories(
        ${OPENGL_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
    )
elseif(WIN32)
    include_directories(
        ${OPENGL_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
    )
endif()

add_executable(main source/main.cpp source/world.cpp source/generators.cpp source/inputs.cpp source/outputs.cpp source/tris.cpp ${GLAD_SOURCE})
target_link_libraries(main glfw OpenGL::GL)

if(UNIX AND NOT APPLE)
    target_link_libraries(main dl)
endif()
