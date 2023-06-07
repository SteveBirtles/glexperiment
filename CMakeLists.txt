cmake_minimum_required(VERSION 3.10)
project(MyOpenGLProject)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-deprecated-volatile)
endif()

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

set(CMAKE_CXX_STANDARD 20)
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
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "armv7l|aarch64") 
        add_compile_definitions(ES_MODE)
    endif()
elseif(WIN32)
    include_directories(
        ${OPENGL_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
    )
endif()



add_executable(main source/main.cpp source/world.cpp source/generators.cpp source/outputs.cpp source/inputs.cpp source/tris.cpp ${GLAD_SOURCE})
target_link_libraries(main glfw OpenGL::GL)

if(UNIX AND NOT APPLE)
    target_link_libraries(main dl)
endif()
