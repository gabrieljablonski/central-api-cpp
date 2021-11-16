#include <iostream>

#include "HttpClient.hpp"

int main() {
  HttpClient client(4440, "192.168.7.118");
  client.auth_login("admin@viacast.tv", "admin1234");
}
