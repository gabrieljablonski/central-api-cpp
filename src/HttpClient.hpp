#pragma once

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace http {

typedef nlohmann::json Body;
typedef std::list<std::string> Headers;

enum class Action { GET, POST, PUT, PATCH, DELETE };

std::string action_to_string(Action action);

}  // namespace http

class HttpClient {
 private:
  int port;
  std::string host;
  std::string prefix;
  int timeout;
  std::string locale;
  bool https;
  std::string token;
  std::string refresh_token;

  std::string build_url(std::string path);
  http::Headers build_headers();
  int request(std::string path, http::Action action = http::Action::GET,
              http::Body body = {});

 public:
  HttpClient(int port, std::string host, std::string prefix = "/v1",
             int timeout = 3000, std::string locale = "en", bool https = false);
  ~HttpClient();

  int auth_login(std::string key, std::string password);
};