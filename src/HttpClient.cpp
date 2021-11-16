#include "HttpClient.hpp"

#include <nlohmann/json.hpp>

std::string http::action_to_string(http::Action action) {
  switch (action) {
    case Action::GET:
      return "GET";
    case Action::POST:
      return "POST";
    case Action::PUT:
      return "PUT";
    case Action::PATCH:
      return "PATCH";
    case Action::DELETE:
      return "DELETE";
  }
  return "";
}

HttpClient::HttpClient(int port, std::string host, std::string prefix,
                       int timeout, std::string locale, bool https)
    : port(port),
      host(host),
      prefix(prefix),
      timeout(timeout),
      locale(locale),
      https(https) {
  curlpp::initialize(CURL_GLOBAL_ALL);
}

HttpClient::~HttpClient() { curlpp::terminate(); }

std::string HttpClient::build_url(std::string path) {
  std::stringstream url;
  url << "http" << (this->https ? "s" : "") << "://" << this->host << ":"
      << this->port << this->prefix << path;
  return url.str();
}

http::Headers HttpClient::build_headers() {
  http::Headers headers;
  std::stringstream accept_language;
  std::stringstream authorization;
  accept_language << "Accept-Language: " << this->locale;
  authorization << "Bearer " << this->token;
  headers.push_back("Content-Type: application/json");
  headers.push_back(accept_language.str());
  if (!this->token.empty()) {
    headers.push_back(authorization.str());
  }
  return headers;
}

// std::string HttpClient::parse_body(http::Body body) {

// }

int HttpClient::request(std::string path, http::Action action,
                        http::Body body) {
  curlpp::Easy request;

  using namespace curlpp::Options;
  request.setOpt(Verbose(true));
  request.setOpt(Url(this->build_url(path)));
  request.setOpt(HttpHeader(this->build_headers()));
  request.setOpt(CustomRequest(http::action_to_string(action)));

  if (!body.empty()) {
    std::string str_body = body.dump();
    request.setOpt(PostFields(str_body));
    request.setOpt(PostFieldSize(str_body.length()));
  }

  request.perform();

  return 0;
}

int HttpClient::auth_login(std::string key, std::string password) {
  return this->request("/auth/login", http::Action::POST,
                       {{"key", key}, {"password", password}});
}
