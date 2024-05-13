cmake_minimum_required(VERSION 3.16...3.26)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header
                                  -Wno-comma>)
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON
               AUTOUIC ON
               AUTORCC ON)
endif()

set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if (${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
	set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src/ui_macos")
else()
	set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src/ui_windows")
endif()


find_package(Qt6Core REQUIRED)
find_package(Qt6Network REQUIRED)	
find_package(Qt6Widgets REQUIRED)

set(ptzcontroller_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/helpwin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hotkeyhelp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xboxhelp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pantiltlimitdlg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/PTZController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/multicameras.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/aboutus.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/joystickdlg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/joystickwrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/QJoysticks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SDL_Joysticks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/VirtualJoystick.cpp
	)

set(ptzcontroller_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/helpwin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hotkeyhelp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xboxhelp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pantiltlimitdlg.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/PTZController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/multicameras.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/aboutus.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/joystickdlg.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/joystickwrapper.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/JoysticksCommon.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/QJoysticks.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SDL_Joysticks.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/VirtualJoystick.h
	)

add_library(${CMAKE_PROJECT_NAME} MODULE
	${ptzcontroller_SOURCES}
	${ptzcontroller_HEADERS}
	${CMAKE_CURRENT_SOURCE_DIR}/src/ptzqrc.qrc
	)

if (WIN32)
    target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "/WX-")
else()
    target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-w")
endif()

find_package(SDL2 REQUIRED)
	
include_directories(
	"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
	${LIBOBS_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
	${Qt6Core_INCLUDES}
	${Qt6Network_INCLUDES}
	${Qt6Widgets_INCLUDES}
	)
		
target_link_libraries(${CMAKE_PROJECT_NAME}
	PRIVATE OBS::frontend-api
	PRIVATE OBS::libobs
	${SDL2_LIBRARIES}
	Qt6::Core
	Qt6::Network
	Qt6::Widgets
	)

setup_plugin_target(${CMAKE_PROJECT_NAME})

