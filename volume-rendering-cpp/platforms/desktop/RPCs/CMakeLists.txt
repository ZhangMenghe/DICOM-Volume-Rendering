# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ helloworld example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building helloworld.

cmake_minimum_required(VERSION 3.5.1)

find_package(Threads REQUIRED)

if(GRPC_AS_SUBMODULE)
  # One way to build a projects that uses gRPC is to just include the
  # entire gRPC project tree via "add_subdirectory".
  # This approach is very simple to use, but the are some potential
  # disadvantages:
  # * it includes gRPC's CMakeLists.txt directly into your build script
  #   without and that can make gRPC's internal setting interfere with your
  #   own build.
  # * depending on what's installed on your system, the contents of submodules
  #   in gRPC's third_party/* might need to be available (and there might be
  #   additional prerequisites required to build them). Consider using
  #   the gRPC_*_PROVIDER options to fine-tune the expected behavior.
  #
  # A more robust approach to add dependency on gRPC is using
  # cmake's ExternalProject_Add (see cmake_externalproject/CMakeLists.txt).
  
  # Include the gRPC's cmake build (normally grpc source code would live
  # in a git submodule called "third_party/grpc", but this example lives in
  # the same repository as gRPC sources, so we just look a few directories up)
  add_subdirectory(../../.. ${BINARY_DIR}/grpc EXCLUDE_FROM_ALL)
  message(STATUS "Using gRPC via add_subdirectory.")

  # After using add_subdirectory, we can now use the grpc targets directly from
  # this build.
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  set(_REFLECTION grpc++_reflection)
  if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
  else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  endif()
  set(_GRPC_GRPCPP grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
  endif()
elseif(GRPC_FETCHCONTENT)
  # Another way is to use CMake's FetchContent module to clone gRPC at
  # configure time. This makes gRPC's source code available to your project,
  # similar to a git submodule.
  message(STATUS "Using gRPC via add_subdirectory (FetchContent).")
  include(FetchContent)
  FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    # when using gRPC, you will actually set this to an existing tag, such as
    # v1.25.0, v1.26.0 etc..
    # For the purpose of testing, we override the tag used to the commit
    # that's currently under test.
    GIT_TAG        vGRPC_TAG_VERSION_OF_YOUR_CHOICE)
  FetchContent_MakeAvailable(grpc)

  # Since FetchContent uses add_subdirectory under the hood, we can use
  # the grpc targets directly from this build.
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  set(_REFLECTION grpc++_reflection)
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
  set(_GRPC_GRPCPP grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
  endif()
else()
  # This branch assumes that gRPC and all its dependencies are already installed
  # on this system, so they can be located by find_package().

  # Find Protobuf installation
  # Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
  set(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf CONFIG REQUIRED)
  message(STATUS "Using protobuf ${protobuf_VERSION}")

  set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
  set(_REFLECTION gRPC::grpc++_reflection)
  if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
  else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  endif()

  # Find gRPC installation
  # Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using gRPC ${gRPC_VERSION}")

  set(_GRPC_GRPCPP gRPC::grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
  endif()
endif()

# Proto file
get_filename_component(cm_proto "${CMAKE_CURRENT_SOURCE_DIR}/RPCs/proto/common.proto" ABSOLUTE)

get_filename_component(ip_proto "${CMAKE_CURRENT_SOURCE_DIR}/RPCs/proto/inspectorSync.proto" ABSOLUTE)

get_filename_component(tm_proto "${CMAKE_CURRENT_SOURCE_DIR}/RPCs/proto/transManager.proto" ABSOLUTE)

get_filename_component(proto_path "${CMAKE_CURRENT_SOURCE_DIR}/RPCs/proto/" PATH)

set(BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RPCs/proto)
# Generated sources
set(cm_proto_srcs "${BINARY_DIR}/common.pb.cc")
set(cm_proto_hdrs "${BINARY_DIR}/common.pb.h")
set(cm_grpc_srcs "${BINARY_DIR}/common.grpc.pb.cc")
set(cm_grpc_hdrs "${BINARY_DIR}/common.grpc.pb.h")

set(ip_proto_srcs "${BINARY_DIR}/inspectorSync.pb.cc")
set(ip_proto_hdrs "${BINARY_DIR}/inspectorSync.pb.h")
set(ip_grpc_srcs "${BINARY_DIR}/inspectorSync.grpc.pb.cc")
set(ip_grpc_hdrs "${BINARY_DIR}/inspectorSync.grpc.pb.h")

set(tm_proto_srcs "${BINARY_DIR}/transManager.pb.cc")
set(tm_proto_hdrs "${BINARY_DIR}/transManager.pb.h")
set(tm_grpc_srcs "${BINARY_DIR}/transManager.grpc.pb.cc")
set(tm_grpc_hdrs "${BINARY_DIR}/transManager.grpc.pb.h")

add_custom_command(
      OUTPUT 
      "${ip_proto_srcs}" "${ip_proto_hdrs}" "${ip_grpc_srcs}" "${ip_grpc_hdrs}" 
      "${tm_proto_srcs}" "${tm_proto_hdrs}" "${tm_grpc_srcs}" "${tm_grpc_hdrs}"
      "${cm_proto_srcs}" "${cm_proto_hdrs}" "${cm_grpc_srcs}" "${cm_grpc_hdrs}" 
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${BINARY_DIR}"
        --cpp_out "${BINARY_DIR}"
        -I "${proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${ip_proto}" "${tm_proto}" "${cm_proto}"
      DEPENDS "${ip_proto}" "${tm_proto}" "${cm_proto}")

SET(proto_srcs
    ${tm_proto_srcs}
    ${ip_proto_srcs}
    ${cm_proto_srcs}
)
SET(grpc_srcs
    ${tm_grpc_srcs}
    ${ip_grpc_srcs}
    ${cm_grpc_srcs}
)     

# Include generated *.pb.h files
include_directories("${proto_path}")

