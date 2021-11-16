cmake_minimum_required(VERSION 3.0)

project(CentralApi)

add_library(central-api src/HttpClient.cpp)
add_executable(api-test src/api-test.cpp)

target_link_libraries(central-api -lcurl -lcurlpp)
target_link_libraries(api-test central-api)
