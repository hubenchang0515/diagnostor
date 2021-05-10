cmake_minimum_required(VERSION 3.0.0)

project(diagnostor VERSION 0.1 LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

# 设置编译选项
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# 添加Qt的模块
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5DBus REQUIRED)

# Qt DBus Factory
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)


# 设置源码目录
aux_source_directory(./core CORE_SRC)
aux_source_directory(./module MODULE_SRC)
aux_source_directory(./utility UTILITY_SRC)


# 添加头文件目录
include_directories(
    ./core
    ./module
    ./utility
    ${DFrameworkDBus_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
    ${CORE_SRC}
    ${MODULE_SRC}
    ${UTILITY_SRC}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Sql
    Qt5::DBus
    ${DFrameworkDBus_LIBRARIES}
)
