#include <unistd.h>

#include <iostream>

#include "central-api.hpp"
#include "types.hpp"
#include "utils.hpp"

int main() {
  using namespace viacast::central;
  CentralApi client(4440, "192.168.7.118", "/v1", 3000, "en", false);
  auto r = client.auth_login("0872", "9FxH8d1GenGmZhVv6zbam1").get();

  if (!r.success) {
    std::cout << "Login failed: '" << r.message << "'" << std::endl;
    exit(-1);
  }

  while (!client.socket_connected()) {
    auto c = client.socket_connect();
    std::cout << "waiting client connection" << std::endl;
    auto connected = c.get();
    if (connected.empty()) {
      std::cout << "client connected" << std::endl;
    } else {
      std::cout << "client connection failed: '" << connected << "'"
                << std::endl;
      client.socket_disconnect();
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
