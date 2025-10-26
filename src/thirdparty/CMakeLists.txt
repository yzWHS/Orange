# thirdparty 模块
cmake_minimum_required(VERSION 3.5)

# 查找第三方库的源文件（如果有）
file(GLOB THIRDPARTY_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

# 查找头文件
file(GLOB THIRDPARTY_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# 如果有源码文件，创建库
if(THIRDPARTY_SOURCES)
    add_library(OrangeThirdParty STATIC
        ${THIRDPARTY_SOURCES}
        ${THIRDPARTY_HEADERS}
    )
    
    # 包含目录
    target_include_directories(OrangeThirdParty PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
else()
    # 如果只有头文件，创建接口库
    add_library(OrangeThirdParty INTERFACE)
    target_include_directories(OrangeThirdParty INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()