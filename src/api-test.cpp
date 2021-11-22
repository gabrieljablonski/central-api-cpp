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

  client.socket_try_connect();
  client.device_on_request_ownership(
      [](socket::DeviceOnRequestOwnershipCallbackArgs args) {
        std::cout << "ownership requested: " << args.code << " (expires on "
                  << args.expiration << ")" << std::endl;
      });
  utils::wait_forever();
  return 0;
}
