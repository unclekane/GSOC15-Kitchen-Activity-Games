cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(KitchenActivityGames)
#add_subdirectory(src/clientPlugin_helloWorld)
add_subdirectory(src/serverPlugin_helloWorld)

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
QT4_WRAP_CPP(headers_MOC include/MainWindow.hh
                         include/PlayList.hh )


#find_package(ignition-math QUIET REQUIRED)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-MATH_CXX_FLAGS}")
#include_directories(${IGNITION-MATH_INCLUDE_DIRS})
#link_directories(${IGNITION-MATH_LIBRARY_DIRS})



add_executable(gzserver src/ServerMain src/ServerMain.cc)
target_link_libraries(gzserver ${GAZEBO_LIBRARIES} gazebo_util ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${QT_LIBRARIES} QtTest pthread)


add_executable(KitchenActivityGamesGUI src/MinimalClientMain.cc src/MainWindow.cc src/PlayList.cc  ${headers_MOC} )
target_link_libraries(KitchenActivityGamesGUI ${GAZEBO_LIBRARIES} gazebo_util ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE-Terrain_LIBRARIES} ${PROTOBUF_LIBRARIES} ${QT_LIBRARIES} ${IGNITION-MATH_LIBRARIES} QtTest pthread)
