cmake_minimum_required(VERSION 3.0)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(CentralApi)

add_library(central-api src/http.cpp src/socket.cpp src/types.cpp)
add_executable(api-test src/api-test.cpp)

target_link_libraries(central-api -lcurl -lcurlpp -pthread -lsioclient)
target_link_libraries(api-test central-api)
