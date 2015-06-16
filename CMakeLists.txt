cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(KitchenActivityGames)

# Find packages
find_package (Qt4)

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)
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
QT4_WRAP_CPP(headers_MOC include/MainWindow.hh)


add_executable(gzserver src/ServerMain src/ServerMain.cc)
target_link_libraries(gzserver ${GAZEBO_LIBRARIES} gazebo_util ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${QT_LIBRARIES} QtTest pthread)


add_executable(MinimalClient src/MinimalClientMain.cc src/MainWindow.cc ${headers_MOC} )
target_link_libraries(MinimalClient ${GAZEBO_LIBRARIES} gazebo_util ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE-Terrain_LIBRARIES} ${PROTOBUF_LIBRARIES} ${QT_LIBRARIES} QtTest pthread)
