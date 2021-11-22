cmake_minimum_required(VERSION 3.0)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(CentralApi)

add_library(centralapi src/version.cpp src/utils.cpp src/types.cpp src/http.cpp src/socket.cpp)
add_executable(api-test src/api-test.cpp)

target_link_libraries(centralapi -lcurl -lcurlpp -pthread -lsioclient)
target_link_libraries(api-test centralapi)
