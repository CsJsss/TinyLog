cmake_minimum_required(VERSION 3.2)
project(TinyLog VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CXX_FLAGS
    -g
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -Wall
    -Wextra
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
    -rdynamic
 )

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

# 查询所有在src中的 .h 和 .cpp文件
file(GLOB sources CMAKE_CONFIGURE_DEPENDS src/*.cpp include/*.h)


# 通过CMKAE_CXX_STANDARD 设置cpp版本, 比add_compile_options 更安全
set(CMAKE_CXX_STANDARD 17)

# 生成 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# 设置打印日志等级
set(ENV{TINY_LOG_TRACE} "TRUE")

# 判断TINY_LOG_TRACE环境变量是否定义
if(DEFINED ENV{TINY_LOG_TRACE})
    message("TINY_LOG_TRACE: $ENV{TINY_LOG_TRACE}")
endif()


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


MESSAGE( STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}.")
MESSAGE( STATUS "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}.")

# 静态库
add_library(TinyLog STATIC ${sources})
target_include_directories(TinyLog PUBLIC ${PROJECT_SOURCE_DIR})