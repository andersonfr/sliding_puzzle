cmake_minimum_required(VERSION 3.11)
project(sliding_puzzle)

# Generate compile_comands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#c++ version
set(CMAKE_CXX_STANDARD 20)

# Dependencies
set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) #QUIET or REQUIRED
if(NOT raylib_FOUND) # if there's none, fetch and build raylib
   include(FetchContent)
   FetchContent_Declare(
      raylib
      URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
   )
   FetchContent_GetProperties(raylib)
   if(NOT raylib_POPULATED) # Have we downloaded raylib yet?
     set(FETCHCONTENT_QUIET NO)
     FetchContent_Populate(raylib)
     set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
     add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
   endif()
endif()
# Our Project

# JSON API
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(SOURCES 
    "src/main.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PRIVATE "include")
include_directories("${CMAKE_SOURCE_DIR}/include")

target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# Web Configurations
if($PLATFORM} STREQUAL "Web")
   # Tell Emscripten to build an example.html file.
   set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()