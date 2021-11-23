cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s")

project(CentralApi)

add_library(centralapi src/version.cpp src/utils.cpp src/types.cpp src/http.cpp src/socket.cpp src/central-api.cpp)
add_executable(api-test src/api-test.cpp)

target_link_libraries(centralapi -lcurl -lcurlpp -pthread -lsioclient)
target_link_libraries(api-test centralapi)

set_target_properties(centralapi PROPERTIES PUBLIC_HEADER "src/central-api.hpp;src/http.hpp;src/socket.hpp;src/types.hpp;src/utils.hpp;src/version.hpp")
INSTALL(
  TARGETS centralapi
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/central-api
)
