cmake_minimum_required(VERSION 3.4.1)

project (vrDesktop)

set(SRC_FILES
	main.cpp
	utils/dicomLoader.cpp
	utils/uiController.cpp)
	
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DRESOURCE_DESKTOP_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/")

add_executable(
	vrDesktop
	${SRC_FILES}
)
target_link_libraries(
	vrDesktop
	vrNative
)