cmake_minimum_required(VERSION 3.6...4.0)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_NAME)   
string(REPLACE " " "_" ProjectId ${PROJECT_NAME})
project(${PROJECT_NAME} LANGUAGES CXX)

# set(CMAKE_C_COMPILER "/usr/bin/clang")
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)
# set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)

option(USE_LIBASSERT "Use libassert" ON)

cmake_minimum_required(VERSION 3.6...4.0)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_NAME)   
string(REPLACE " " "_" ProjectId ${PROJECT_NAME})
project(${PROJECT_NAME} LANGUAGES CXX)

# set(CMAKE_C_COMPILER "/usr/bin/clang")
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)
# set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)

option(USE_LIBASSERT "Use libassert" ON)

function(add_git_dependency libName gitURL gitTag)
  FetchContent_Declare(
    ${libName}
    GIT_REPOSITORY ${gitURL}
    GIT_TAG ${gitTag}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  if(NOT ("${libName}" STREQUAL "libname"))
    FetchContent_MakeAvailable(${libName})
  endif()
endfunction()

if(USE_LIBASSERT)
  set(dep libassert)
  add_git_dependency(${dep} https://github.com/jeremy-rifkin/libassert.git
                     v2.1.5)
endif()

#------------------------------------------------------------------------
# llvm stuff
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
#------------------------------------------------------------------------

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*compiler_test\\.cpp$")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if(USE_LIBASSERT)
  FetchContent_GetProperties(libassert)
  if(libassert_SOURCE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${libassert_SOURCE_DIR}/src)
  endif()
  target_link_libraries(${PROJECT_NAME} PRIVATE LLVM libassert::assert)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE LLVM)
endif()

# NOTE: removing asan because llvm breaks with it, hope that doesnt cause issues later
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "-O0 -g -Werror -Wall -Wno-unused-variable")
endif()
