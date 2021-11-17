#include <iostream>

#include "CentralTypes.hpp"
#include "HttpClient.hpp"

int main() {
  // using namespace viacast::central::entities;
  // using namespace viacast::central::types;

  // User user(nlohmann::json::parse(R"(
  //   {
  //     "id": "1234",
  //     "email": "gabriel@viacast.tv",
  //     "name": "Gabriel Jablonski",
  //     "organization": "Viacast Soluções",
  //     "deviceIds": ["123", "456", "789"]
  //   }
  // )"));

  // for (auto device = user.device_ids.begin(); device !=
  // user.device_ids.end();
  //      ++device) {
  //   std::cout << *device << std::endl;
  // }
  // std::cout << user.to_json().dump() << std::endl;

  viacast::central::http::Client client("192.168.7.118", 4440, "/v1", 3, "en",
                                        false);

  client.auth_login("gabriel@viacast.tv", "").wait();
  // auto r = client.user_me();
  // r.wait();
  // auto response = r.get();
  // std::cout << "success: " << response.success << std::endl;
  // std::cout << "message: " << response.message << std::endl;
  // std::cout << "user: " << response.data.to_json().dump() << std::endl;

  auto r = client.user_my_devices();
  r.wait();
  auto response = r.get();
  std::cout << "success: " << response.success << std::endl;
  std::cout << "message: " << response.message << std::endl;
  std::cout << "devices:" << std::endl;
  for (auto d = response.data;) }
