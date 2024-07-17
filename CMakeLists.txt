cmake_minimum_required(VERSION 3.16...3.26)

legacy_check()

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)	

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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


target_enable_feature(mac-syphon "Syphon sharing support")

add_library(obs-ptzcontroller MODULE)
add_library(OBS::obs-ptzcontroller ALIAS obs-ptzcontroller)

target_sources(obs-ptzcontroller PRIVATE 
    src/helpwin.cpp
    src/hotkeyhelp.cpp
    src/xboxhelp.cpp
    src/pantiltlimitdlg.cpp
	src/PTZController.cpp
	src/mainwindow.cpp
	src/multicameras.cpp
	src/aboutus.cpp
	src/joystickdlg.cpp
	src/joystickwrapper.cpp
	src/QJoysticks.cpp
	src/SDL_Joysticks.cpp
	src/VirtualJoystick.cpp
	src/ptzqrc.qrc	
    src/helpwin.h
    src/hotkeyhelp.h
    src/xboxhelp.h
    src/pantiltlimitdlg.h
	src/PTZController.h
	src/mainwindow.h
	src/multicameras.h
	src/aboutus.h
	src/constants.h
	src/joystickdlg.h
	src/joystickwrapper.h
	src/JoysticksCommon.h
	src/QJoysticks.h
	src/SDL_Joysticks.h
	src/VirtualJoystick.h
	)

target_link_libraries(obs-ptzcontroller PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(obs-ptzcontroller PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
     obs-ptzcontroller PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header
                                  -Wno-comma>)
  set_target_properties(
     obs-ptzcontroller
    PROPERTIES AUTOMOC ON
               AUTOUIC ON
               AUTORCC ON)
endif()
	

	
if (WIN32)
    target_compile_options(obs-ptzcontroller PUBLIC "/WX-")
else()
    target_compile_options(obs-ptzcontroller PUBLIC "-w")
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

target_link_libraries(obs-ptzcontroller
	PRIVATE OBS::frontend-api
	PRIVATE OBS::libobs
	${SDL2_LIBRARIES}
	Qt6::Core
	Qt6::Network
	Qt6::Widgets
	)

set_target_properties_obs(obs-ptzcontroller PROPERTIES FOLDER plugins/ptzoptics-obs PREFIX "")
