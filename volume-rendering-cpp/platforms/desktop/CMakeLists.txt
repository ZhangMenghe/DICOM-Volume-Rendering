cmake_minimum_required(VERSION 3.4.1)

project (vrDesktop)

set(RPC_ENABLED CACHE BOOL True)
set(BUILD_TYPE CACHE STRING "Debug")
set(CMAKE_BUILD_TYPE  ${BUILD_TYPE})
# MESSAGE(STATUS "Build type: "${CMAKE_BUILD_TYPE})
set(SRC_FILES
	main.cpp
)
SET(UTIL_FILES
	utils/dicomLoader.cpp
	utils/uiController.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DRESOURCE_DESKTOP_DIR="${ASSET_DIRECTORY}/")

if(RPC_ENABLED)
	MESSAGE(STATUS "With GRPC Enabled")
	add_definitions(-DRPC_ENABLED)
	include(${CMAKE_CURRENT_SOURCE_DIR}/RPCs/CMakeLists.txt)
	
	add_executable(
		vrDesktop
		${SRC_FILES}
		RPCs/rpcHandler.cpp
		${UTIL_FILES}
		${proto_srcs}
    	${grpc_srcs}
	)
	target_link_libraries(
		vrDesktop
		vrNative
		${_REFLECTION}
		${_GRPC_GRPCPP}
		${_PROTOBUF_LIBPROTOBUF}
	)
else()
	add_executable(
		vrDesktop
		${SRC_FILES}
		${UTIL_FILES}
	)
	target_link_libraries(
		vrDesktop
		vrNative
	)
endif()