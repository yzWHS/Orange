# ui 模块
cmake_minimum_required(VERSION 3.5)

# 查找 UI 模块的源文件
file(GLOB UI_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

# 查找 UI 模块的头文件
file(GLOB UI_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# 创建 UI 库
add_library(OrangeUI STATIC
    ${UI_SOURCES}
    ${UI_HEADERS}
)

# 包含目录
target_include_directories(OrangeUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接依赖
target_link_libraries(OrangeUI PRIVATE 
    Qt6::Widgets
    OrangeThirdParty
)