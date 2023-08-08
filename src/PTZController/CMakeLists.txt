# CMakeList.txt : CMake project for PTZController
cmake_minimum_required (VERSION 3.8)
legacy_check()
project(obs-ptzcontroller)

set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
if (${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
	set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui_macos")
else()
	set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui_windows")
endif()


find_package(Qt6Core REQUIRED)
find_package(Qt6Network REQUIRED)	
find_package(Qt6Widgets REQUIRED)


set(ptzcontroller_SOURCES
    helpwin.cpp
    hotkeyhelp.cpp
    xboxhelp.cpp
    pantiltlimitdlg.cpp
	PTZController.cpp
	mainwindow.cpp
	multicameras.cpp
	aboutus.cpp
	joystickdlg.cpp
	joystickwrapper.cpp
	QJoysticks.cpp
	SDL_Joysticks.cpp
	VirtualJoystick.cpp
	)

set(ptzcontroller_HEADERS
    helpwin.h
    hotkeyhelp.h
    xboxhelp.h
    pantiltlimitdlg.h
	PTZController.h
	mainwindow.h
	multicameras.h
	aboutus.h
	constants.h
	joystickdlg.h
	joystickwrapper.h
	JoysticksCommon.h
	QJoysticks.h
	SDL_Joysticks.h
	VirtualJoystick.h
	)
	
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
add_library(obs-ptzcontroller MODULE
	${ptzcontroller_SOURCES}
	${ptzcontroller_HEADERS}
	ptzqrc.qrc
	)

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

#target_compile_options(obs-ptzcontroller PUBLIC /WX-)

# cmake-format: off
set_target_properties_obs(obs-ptzcontroller PROPERTIES FOLDER plugins PREFIX "")
# cmake-format: on


