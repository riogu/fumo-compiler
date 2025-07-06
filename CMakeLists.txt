cmake_minimum_required(VERSION 3.6...4.0)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectName)   
string(REPLACE " " "_" ProjectId ${ProjectName})
project(${ProjectName} LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(LIBRARIES ${PROJECT_NAME})

option(USE_LIBASSERT "Use libassert" ON)

function(add_git_dependency libName gitURL gitTag)
  FetchContent_Declare(
    ${libName}
    GIT_REPOSITORY ${gitURL}
    GIT_TAG ${gitTag}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)
  if(NOT ("${libName}" STREQUAL "libname"))
    FetchContent_MakeAvailable(${libName})
  endif()
endfunction()

if(USE_LIBASSERT)
  set(dep libassert)
  add_git_dependency(${dep} https://github.com/jeremy-rifkin/libassert.git
                     v2.1.5)
  include_directories(RECURSE build/_deps/libassert-src/src)
  list(APPEND LIBRARIES libassert::assert)
endif()

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/")
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "-O0 -g -fsanitize=address -Werror -Wall -Wno-unused-variable")
endif()

