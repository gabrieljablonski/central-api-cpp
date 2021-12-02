cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-L/home/jablonski/central-build-nano/lib -I/home/jablonski/central-build-nano/include -std=c++11 -std=gnu++11 -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -O3 -s -DNDEBUG")

project(CentralApi)

add_library(centralapi src/version.cpp src/utils.cpp src/types.cpp src/http.cpp src/socket.cpp src/central-api.cpp)
add_executable(api-test src/api-test.cpp)

target_link_libraries(centralapi -pthread -lcurl -lcurlpp -lsioclient)
target_link_libraries(api-test centralapi)

set_target_properties(centralapi PROPERTIES PUBLIC_HEADER "src/central-api.hpp;src/http.hpp;src/socket.hpp;src/types.hpp;src/utils.hpp;src/version.hpp")
INSTALL(
  TARGETS centralapi
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/central-api
)
