cmake_minimum_required(VERSION 3.0.0)
project(AsynLog VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(TinyLog TinyLog/build/lib)

add_executable(AsynLog main.cpp)


# 包含-pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(AsynLog PRIVATE Threads::Threads TinyLog)

# target_include_directories(AsynLog TinyLog)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
