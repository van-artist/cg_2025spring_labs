cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 添加 glad、GLEW、GLFW 头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/glad) 
include_directories(/opt/homebrew/include)  
link_directories(/opt/homebrew/lib)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/glad.c)


# 生成可执行文件
add_executable(OpenGL_Project ${SOURCES})
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:OpenGL_Project>/shaders
)
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/static $<TARGET_FILE_DIR:OpenGL_Project>/static
)

# 链接 OpenGL、GLEW、GLFW
target_link_libraries(OpenGL_Project OpenGL::GL GLEW glfw)
