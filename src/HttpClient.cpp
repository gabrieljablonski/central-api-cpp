#include "HttpClient.hpp"

#include <nlohmann/json.hpp>

namespace viacast::central::http {

std::string action_to_string(Action action) {
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

AuthInfo::AuthInfo() {}

AuthInfo::AuthInfo(nlohmann::json json) {
  if (json.contains("token")) {
    json.at("token").get_to(this->token);
  }
  if (json.contains("refreshToken")) {
    json.at("refreshToken").get_to(this->refresh_token);
  }
  if (json.contains("verified")) {
    json.at("verified").get_to(this->verified);
  }
}

nlohmann::json UserRegisterArgs::to_json() const {
  nlohmann::json j;
  if (!this->email.empty()) {
    j["email"] = this->email;
  }
  if (!this->name.empty()) {
    j["name"] = this->name;
  }
  if (!this->organization.empty()) {
    j["organization"] = this->organization;
  }
  if (!this->password.empty()) {
    j["password"] = this->password;
  }
  if (!this->captcha_token.empty()) {
    j["captchaToken"] = this->captcha_token;
  }
  return j;
}

UserResponse::UserResponse(entities::User user) : user(user){};
DevicesResponse::DevicesResponse(std::list<entities::Device> devices)
    : devices(devices) {}

nlohmann::json DeviceRegisterArgs::to_json() const {
  nlohmann::json j;
  if (!this->serial.empty()) {
    j["serial"] = this->serial;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  if (!this->model.empty()) {
    j["model"] = this->model;
  }
  if (!this->key.empty()) {
    j["key"] = this->key;
  }
  return j;
}

Client::Client(std::string host, int port, std::string prefix, int timeout,
               std::string locale, bool https)
    : host(host),
      port(port),
      prefix(prefix),
      timeout(timeout),
      locale(locale),
      https(https) {
  curlpp::initialize(CURL_GLOBAL_ALL);
}

Client::~Client() { curlpp::terminate(); }

std::string Client::build_url(std::string path) {
  std::ostringstream url;
  url << "http" << (this->https ? "s" : "") << "://" << this->host << ":"
      << this->port << this->prefix << path;
  return url.str();
}

Headers Client::build_headers() {
  Headers headers;
  std::ostringstream accept_language;
  std::ostringstream authorization;
  accept_language << "Accept-Language: " << this->locale;
  authorization << "Authorization: Bearer " << this->token;
  headers.push_back("Content-Type: application/json");
  headers.push_back(accept_language.str());
  if (!this->token.empty()) {
    headers.push_back(authorization.str());
  }
  return headers;
}

Response<Body> Client::request(std::string path, Action action, Body body) {
  curlpp::Easy request;
  using namespace curlpp::Options;
  // request.setOpt(Verbose(true));
  request.setOpt(ConnectTimeout(this->timeout));
  request.setOpt(Url(this->build_url(path)));
  request.setOpt(HttpHeader(this->build_headers()));
  request.setOpt(CustomRequest(http::action_to_string(action)));
  if (!body.empty()) {
    std::string str_body = body.dump();
    request.setOpt(PostFields(str_body));
    request.setOpt(PostFieldSize(str_body.length()));
  }
  std::ostringstream r;
  request.setOpt(WriteStream(&r));
  try {
    request.perform();
  } catch (curlpp::LibcurlRuntimeError const &e) {
    Response<Body> response = {500, false, e.what(), Body({})};
    return response;
  }
  long http_code = curlpp::infos::ResponseCode::get(request);
  Body rbody = Body({});
  if (!r.str().empty()) {
    rbody = Body::parse(r.str());
  }
  bool success = http_code >= 2000 && http_code < 300;
  std::string message;
  Body data;
  if (rbody.contains("message")) {
    message = rbody["message"].get<std::string>();
  }
  if (rbody.contains("success")) {
    success = rbody["success"].get<bool>();
  }
  if (rbody.contains("data")) {
    data = rbody["data"];
  }
  Response<Body> response = {http_code, success, message, data};
  return response;
}

PromisedResponse<AuthInfo> Client::auth_login(std::string key,
                                              std::string password) {
  return std::async(std::launch::async, [&, this, key, password]() {
    auto r = this->request("/auth/login", Action::POST,
                           {{"key", key}, {"password", password}});
    AuthInfo auth;
    if (r.data.contains("token")) {
      auth.token = r.data.at("token");
      auth.refresh_token = r.data.at("refreshToken");
      this->token = auth.token;
    }
    http::Response<AuthInfo> response = {r.status, r.success, r.message, auth};
    return response;
  });
}

PromisedResponse<> Client::auth_request_verification_code() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/auth/verification-code", Action::GET);
    Response<> response = {r.status, r.success, r.message, Body({})};
    return response;
  });
}

PromisedResponse<> Client::auth_submit_verification_code(std::string code) {
  return std::async(std::launch::async, [&, this, code]() {
    auto r = this->request("/auth/verification-code", Action::POST,
                           {{"code", code}});
    Response<> response = {r.status, r.success, r.message, Body({})};
    return response;
  });
}

PromisedResponse<AuthInfo> Client::auth_refresh_token(
    std::string refresh_token) {
  return std::async(std::launch::async, [&, this, refresh_token]() {
    auto r = this->request("/auth/refresh-token", Action::POST,
                           {{"refreshToken", refresh_token}});
    AuthInfo auth;
    if (r.data.contains("token")) {
      auth.token = r.data.at("token");
      auth.refresh_token = r.data.at("refreshToken");
      this->token = auth.token;
    }
    http::Response<AuthInfo> response = {r.status, r.success, r.message, auth};
    return response;
  });
}

PromisedResponse<> Client::auth_change_password(std::string old_password,
                                                std::string new_password) {
  return std::async(
      std::launch::async, [&, this, old_password, new_password]() {
        auto r = this->request(
            "/auth/change-password", Action::POST,
            {{"oldPassword", old_password}, {"newPassword", new_password}});
        Response<> response = {r.status, r.success, r.message, Body({})};
        return response;
      });
}

PromisedResponse<UserResponse> Client::user_register(UserRegisterArgs user) {
  return std::async(std::launch::async, [&, this, user]() {
    auto r = this->request("/user/register", Action::POST, user.to_json());
    entities::User user;
    if (r.data.contains("user")) {
      user = entities::User(r.data.at("user"));
    }
    Response<UserResponse> response = {r.status, r.success, r.message,
                                       UserResponse(user)};
    return response;
  });
}

PromisedResponse<UserResponse> Client::user_me() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/user/me", Action::GET);
    entities::User user;
    if (r.data.contains("user")) {
      user = entities::User(r.data.at("user"));
    }
    Response<UserResponse> response = {r.status, r.success, r.message,
                                       UserResponse(user)};
    return response;
  });
}

PromisedResponse<DevicesResponse> Client::user_my_devices() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/user/me", Action::GET);
    std::list<entities::Device> devices;
    if (r.data.contains("devices")) {
      auto d = r.data.at("devices");
      for (auto device = d.begin(); device != d.end(); ++device) {
        devices.push_back(entities::Device(*device));
      }
    }
    Response<DevicesResponse> response = {r.status, r.success, r.message,
                                          DevicesResponse(devices)};
    return response;
  });
}

}  // namespace viacast::central::http
