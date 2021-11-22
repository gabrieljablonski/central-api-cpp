#include <unistd.h>

#include <iostream>

#include "http.hpp"
#include "socket.hpp"
#include "types.hpp"
#include "utils.hpp"

int main() {
  using namespace viacast::central;
  // http::Client client(4440, "192.168.7.118", "/v1", 3,
  // "en",
  //                                       false, false);
  // client.auth_login("gabriel@viacast.tv", "").wait();
  // auto r = client.service_toggle_running(
  //     "1234", types::ToggleRunningAction::START);
  // r.wait();
  // auto response = r.get();
  // std::cout << "success: " << response.success << std::endl;
  // std::cout << "message: " << response.message << std::endl;

  socket::Client client(
      4440, "192.168.7.118", "/v1/socket.io/", 3000, "en", false,
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
      "eyJhdXRoSWQiOiJjYTdhM2M5OC03ZjliLTRlYjAtYTNlMi04NjkzMTk0MTI2ZWIiLCJhdXRo"
      "S2V5IjoiMDg3MiIsInVzZXJJZCI6bnVsbCwiZGV2aWNlSWQiOiIwMDJjZjZhYS03N2IzLTQz"
      "NGMtYTU3NS0zZTM3MzQyNzAyODQiLCJpYXQiOjE2Mzc1ODcyNzEsImV4cCI6MTYzNzY3MzY3"
      "MX0.rUDaX5elALq3bGi1Wj6NzOqju0CfmA9nCdea7O4YRpo");

  while (!client.connected()) {
    auto c = client.connect();
    std::cout << "waiting client connection" << std::endl;
    auto connected = c.get();
    if (connected.empty()) {
      std::cout << "client connected" << std::endl;
    } else {
      std::cout << "client connection failed: '" << connected << "'"
                << std::endl;
      client.disconnect();
      sleep(1);
    }
  }

  client.device_on_request_ownership(
      [](socket::DeviceOnRequestOwnershipCallbackArgs args) {
        std::cout << "ownership requested: " << args.code << " (expires on "
                  << args.expiration << ")" << std::endl;
      });

  utils::wait_forever();
  return 0;
}
