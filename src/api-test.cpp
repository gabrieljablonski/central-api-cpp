#include <unistd.h>

#include <iostream>

#include "http.hpp"
#include "socket.hpp"
#include "types.hpp"

int main() {
  using namespace viacast::central;
  // viacast::central::http::Client client(4440, "192.168.7.118", "/v1", 3,
  // "en",
  //                                       false, false);
  // client.auth_login("gabriel@viacast.tv", "").wait();
  // auto r = client.service_toggle_running(
  //     "1234", viacast::central::types::ToggleRunningAction::START);
  // r.wait();
  // auto response = r.get();
  // std::cout << "success: " << response.success << std::endl;
  // std::cout << "message: " << response.message << std::endl;

  socket::Client client(
      4440, "192.168.7.118", "/v1/socket.io/", 3000, "en", false,
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
      "eyJhdXRoSWQiOiJjYTdhM2M5OC03ZjliLTRlYjAtYTNlMi04NjkzMTk0MTI2ZWIiLCJhdXRo"
      "S2V5IjoiMDg3MiIsInVzZXJJZCI6bnVsbCwiZGV2aWNlSWQiOiIwMDJjZjZhYS03N2IzLTQz"
      "NGMtYTU3NS0zZTM3MzQyNzAyODQiLCJpYXQiOjE2MzcyNjE3ODcsImV4cCI6MTYzNzM0ODE4"
      "N30.Y5bJOzw-DSBYbXle97M4rw3cm392ujp4yu4aKx1wfu");
  client.connect([]() { std::cout << "connected socket" << std::endl; },
                 []() { std::cout << "failed socket connection" << std::endl; },
                 [](sio::client::close_reason reason) {
                   std::cout << "closed " << reason << std::endl;
                 });
  sleep(1);
  client.emit(socket::Event::TEST_EVENT, nlohmann::json::parse(R"(
                {
                  "false": false,
                  "float": 1.23,
                  "int": 123,
                  "object": { "test_nested": 123 },
                  "string": "123",
                  "true": true,
                  "array": [{"oi": "123"}, 123, 1.23, true, false]
                }
              )"));
  sleep(9999999);
}
