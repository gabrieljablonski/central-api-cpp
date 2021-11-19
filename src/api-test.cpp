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

  auto client = std::make_shared<socket::Client>(
      4440, "192.168.7.118", "/v1/socket.io/", 3000, "en", false,
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
      "eyJhdXRoSWQiOiIxZGRkZDcwNC0zMzc3LTRlY2ItOGRjZS1lZmRhZTUwOGZkNTIiLCJhdXRo"
      "S2V5IjoiYWRtaW5AdmlhY2FzdC50diIsImlhdCI6MTYzNzMzNzc4NSwiZXhwIjoxNjM3NDI0"
      "MTg1fQ.HL5ScK6TUXyX96tpYuqxnWyk8OxlSKbqEclDwCOHnyM");
  client->connect(
      [](socket::Client* client) {
        std::cout << "connected" << std::endl;
        sleep(1);
        auto r =
            client->emit(socket::Event::TEST_EVENT, nlohmann::json::parse(R"(
                {
                  "true": true,
                  "false": false,
                  "int": 123,
                  "float": 1.23,
                  "string": "123",
                  "object": { "test_nested": 123 },
                  "array": [{"oi": "123"}, 123, 1.23, true, false]
                }
              )"));
        std::cout << "waiting response" << std::endl;
        r.wait();
        std::cout << "r " << r.get().dump() << std::endl;
      },
      [](std::string reason) {
        std::cout << "failed socket connection: " << reason << std::endl;
      },
      []() { std::cout << "closed" << std::endl; });
  // sleep(1);
  // auto r = client->emit(socket::Event::TEST_EVENT, nlohmann::json::parse(R"(
  //               {
  //                 "true": true,
  //                 "false": false,
  //                 "int": 123,
  //                 "float": 1.23,
  //                 "string": "123",
  //                 "object": { "test_nested": 123 },
  //                 "array": [{"oi": "123"}, 123, 1.23, true, false]
  //               }
  //             )"));
  // std::cout << "waiting response" << std::endl;
  // r.wait();
  // std::cout << "r " << r.get().dump() << std::endl;
  utils::wait_forever();

  return 0;
}
