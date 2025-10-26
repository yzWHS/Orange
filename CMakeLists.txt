cmake_minimum_required(VERSION 3.5)
project(Orange LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "D:/Soft/Qt/6.9.1/mingw_64")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 添加子目录
add_subdirectory(src/thirdparty)
add_subdirectory(src/ui)
add_subdirectory(src/app)

# 主可执行文件
add_executable(${PROJECT_NAME} WIN32)

# 链接所有模块
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    OrangeUI
    OrangeApp
)