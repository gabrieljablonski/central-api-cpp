cmake_minimum_required(VERSION 3.0)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(CentralApi)

add_library(http-client src/http.cpp src/types.cpp)
add_executable(api-test src/api-test.cpp)

target_link_libraries(http-client -lcurl -lcurlpp -pthread)
target_link_libraries(api-test http-client)
