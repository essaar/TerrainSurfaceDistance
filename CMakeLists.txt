cmake_minimum_required(VERSION 3.14)

project(TerrainSurfaceDistance LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(TerrainSurfaceDistance
  main.cpp
  pre.data
  post.data
)
target_link_libraries(TerrainSurfaceDistance Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(TerrainSurfaceDistance PRIVATE include)

include(GNUInstallDirs)
install(TARGETS TerrainSurfaceDistance
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
