cmake_minimum_required(VERSION 3.5)

# 查找其他第三方源码
file(GLOB THIRDPARTY_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

file(GLOB THIRDPARTY_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(OrangeThirdParty STATIC
        ${THIRDPARTY_SOURCES}
        ${THIRDPARTY_HEADERS}
)

# 包含目录
target_include_directories(OrangeThirdParty PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include # spdlog 库头文件
)