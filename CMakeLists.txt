cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(Kitchen_Activity_Games)

# Find packages
find_package (Qt4)

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


find_package(Protobuf REQUIRED)

include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GAZEBO gazebo)
  pkg_check_modules(OGRE OGRE)
  pkg_check_modules(OGRE-Terrain OGRE-Terrain)
endif()
string (REPLACE ";" " " GAZEBO_CFLAGS "${GAZEBO_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CFLAGS}")

include (${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# include appropriate directories
include_directories(SYSTEM ${GAZEBO_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS} ${OGRE-Terrain_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})

# Create libraries and executables

QT4_WRAP_CPP(headers_MOC main_window.hh)


add_executable(server_main server_main.cc)
target_link_libraries(server_main ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${QT_LIBRARIES} QtTest pthread)


add_executable(client_main client_main.cc main_window.cc ${headers_MOC} )
target_link_libraries(client_main ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE-Terrain_LIBRARIES} ${PROTOBUF_LIBRARIES} ${QT_LIBRARIES} QtTest pthread)
