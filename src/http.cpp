#include "http.hpp"

namespace viacast {
namespace central {
namespace http {

std::string action_to_string(Action action) {
  std::map<Action, std::string> names = {{Action::GET, "GET"},
                                         {Action::POST, "POST"},
                                         {Action::PUT, "PUT"},
                                         {Action::PATCH, "PATCH"},
                                         {Action::DELETE, "DELETE"}};
  return names.at(action);
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

UserResponse::UserResponse(entities::User user) : user(user) {}

DevicesResponse::DevicesResponse(std::list<entities::Device> devices)
    : devices(devices) {}

ServicesResponse::ServicesResponse(std::list<entities::Service> services)
    : services(services) {}

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

DeviceResponse::DeviceResponse(entities::Device device) : device(device) {}

DeviceKeygenResponse::DeviceKeygenResponse() {}

DeviceKeygenResponse::DeviceKeygenResponse(std::string serial, std::string key)
    : serial(serial), key(key) {}

ServiceResponse::ServiceResponse(entities::Service service)
    : service(service) {}

Client::Client() {}

Client::Client(int port, std::string host, std::string prefix, int timeout,
               std::string locale, bool https, bool verbose)
    : port(port),
      host(host),
      prefix(prefix),
      timeout(timeout),
      locale(locale),
      https(https),
      verbose(verbose) {
  curlpp::initialize(CURL_GLOBAL_ALL);
}

Client::~Client() { curlpp::terminate(); }

std::string Client::build_url(std::string path) {
  return "http" + std::string(this->https ? "s" : "") + "://" + this->host +
         ":" + std::to_string(this->port) + this->prefix + path;
}

Headers Client::build_headers() {
  Headers headers;
  headers.push_back("Content-Type: application/json");
  headers.push_back("Accept-Language: " + this->locale);
  if (!this->token.empty()) {
    headers.push_back("Authorization: Bearer " + this->token);
  }
  return headers;
}

Response<nlohmann::json> Client::request(std::string path, Action action,
                                         nlohmann::json body) {
  curlpp::Easy request;
  using namespace curlpp::Options;
  request.setOpt(Verbose(this->verbose));
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
    Response<nlohmann::json> response = {500, false, e.what(), {}};
    return response;
  }
  long http_code = curlpp::infos::ResponseCode::get(request);
  nlohmann::json rbody = {};
  if (!r.str().empty()) {
    rbody = nlohmann::json::parse(r.str());
  }
  bool success = http_code >= 2000 && http_code < 300;
  std::string message;
  nlohmann::json data;
  if (rbody.contains("message")) {
    message = rbody["message"].get<std::string>();
  }
  if (rbody.contains("success")) {
    success = rbody["success"].get<bool>();
  }
  if (rbody.contains("data")) {
    data = rbody["data"];
  }
  Response<nlohmann::json> response = {http_code, success, message, data};
  return response;
}

Future<AuthInfo> Client::auth_login(std::string key, std::string password) {
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

Future<> Client::auth_request_verification_code() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/auth/verification-code", Action::GET);
    Response<> response = {r.status, r.success, r.message, {}};
    return response;
  });
}

Future<> Client::auth_submit_verification_code(std::string code) {
  return std::async(std::launch::async, [&, this, code]() {
    auto r = this->request("/auth/verification-code", Action::POST,
                           {{"code", code}});
    Response<> response = {r.status, r.success, r.message, {}};
    return response;
  });
}

Future<AuthInfo> Client::auth_refresh_token(std::string refresh_token) {
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

Future<> Client::auth_change_password(std::string old_password,
                                      std::string new_password) {
  return std::async(
      std::launch::async, [&, this, old_password, new_password]() {
        auto r = this->request(
            "/auth/change-password", Action::POST,
            {{"oldPassword", old_password}, {"newPassword", new_password}});
        Response<> response = {r.status, r.success, r.message, {}};
        return response;
      });
}

Future<UserResponse> Client::user_register(UserRegisterArgs user) {
  return std::async(std::launch::async, [&, this, user]() {
    auto r = this->request("/user/register", Action::POST, user.to_json());
    entities::User new_user;
    if (r.data.contains("user")) {
      new_user = entities::User(r.data.at("user"));
    }
    Response<UserResponse> response = {r.status, r.success, r.message,
                                       UserResponse(new_user)};
    return response;
  });
}

Future<UserResponse> Client::user_me() {
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

Future<DevicesResponse> Client::user_my_devices() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/user/me/devices", Action::GET);
    std::list<entities::Device> devices;
    if (r.data.contains("devices")) {
      auto d = r.data.at("devices");
      for (auto device : d) {
        devices.push_back(entities::Device(device));
      }
    }
    Response<DevicesResponse> response = {r.status, r.success, r.message,
                                          DevicesResponse(devices)};
    return response;
  });
}

Future<ServicesResponse> Client::user_my_services() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/user/me/services", Action::GET);
    std::list<entities::Service> services;
    if (r.data.contains("services")) {
      auto s = r.data.at("services");
      for (auto service : s) {
        services.push_back(entities::Service(service));
      }
    }
    Response<ServicesResponse> response = {r.status, r.success, r.message,
                                           ServicesResponse(services)};
    return response;
  });
}

Future<DeviceResponse> Client::device_register(DeviceRegisterArgs device) {
  return std::async(std::launch::async, [&, this, device]() {
    auto r = this->request("/device/register", Action::POST, device.to_json());
    entities::Device new_device;
    if (r.data.contains("device")) {
      new_device = entities::Device(r.data.at("device"));
    }
    Response<DeviceResponse> response = {r.status, r.success, r.message,
                                         DeviceResponse(new_device)};
    return response;
  });
}

Future<DeviceResponse> Client::device_me() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/device/me", Action::GET);
    entities::Device device;
    if (r.data.contains("device")) {
      device = entities::Device(r.data.at("device"));
    }
    Response<DeviceResponse> response = {r.status, r.success, r.message,
                                         DeviceResponse(device)};
    return response;
  });
}

Future<ServicesResponse> Client::device_my_services() {
  return std::async(std::launch::async, [&, this]() {
    auto r = this->request("/device/me/services", Action::GET);
    std::list<entities::Service> services;
    if (r.data.contains("services")) {
      auto s = r.data.at("services");
      for (auto service : s) {
        services.push_back(entities::Service(service));
      }
    }
    Response<ServicesResponse> response = {r.status, r.success, r.message,
                                           ServicesResponse(services)};
    return response;
  });
}

Future<DeviceResponse> Client::device_update_me(entities::Device device) {
  return std::async(std::launch::async, [&, this, device]() {
    auto r = this->request("/device/me", Action::PATCH, device.to_json());
    entities::Device updated_device;
    if (r.data.contains("device")) {
      updated_device = entities::Device(r.data.at("device"));
    }
    Response<DeviceResponse> response = {r.status, r.success, r.message,
                                         DeviceResponse(updated_device)};
    return response;
  });
}

Future<DeviceResponse> Client::device_update(entities::Device device) {
  return std::async(std::launch::async, [&, this, device]() {
    auto r =
        this->request("/device/" + device.id, Action::PATCH, device.to_json());
    entities::Device updated_device;
    if (r.data.contains("device")) {
      updated_device = entities::Device(r.data.at("device"));
    }
    Response<DeviceResponse> response = {r.status, r.success, r.message,
                                         DeviceResponse(updated_device)};
    return response;
  });
}

Future<DeviceKeygenResponse> Client::device_keygen(std::string serial,
                                                   bool force_new) {
  return std::async(std::launch::async, [&, this, serial, force_new]() {
    auto r = this->request("/device/keygen", Action::POST,
                           {{"serial", serial}, {"forceNew", force_new}});
    DeviceKeygenResponse keygen_response;
    if (r.data.contains("serial")) {
      r.data.at("serial").get_to(keygen_response.serial);
      r.data.at("key").get_to(keygen_response.key);
    }
    Response<DeviceKeygenResponse> response = {r.status, r.success, r.message,
                                               keygen_response};
    return response;
  });
}

Future<> Client::device_request_ownership(std::string serial) {
  return std::async(std::launch::async, [&, this, serial]() {
    auto r = this->request("/device/request-ownership", Action::POST,
                           {{"serial", serial}});
    Response<> response = {r.status, r.success, r.message, {}};
    return response;
  });
}

Future<DeviceResponse> Client::device_submit_ownership_code(
    std::string serial, std::string code, bool take_ownership) {
  return std::async(
      std::launch::async, [&, this, serial, code, take_ownership]() {
        auto r = this->request("/device/submit-ownership-code", Action::POST,
                               {{"serial", serial},
                                {"code", code},
                                {"takeOwnership", take_ownership}});
        entities::Device device;
        if (r.data.contains("device")) {
          device = entities::Device(r.data.at("device"));
        }
        Response<DeviceResponse> response = {r.status, r.success, r.message,
                                             DeviceResponse(device)};
        return response;
      });
}

Future<ServiceResponse> Client::service_register(entities::Service service) {
  return std::async(std::launch::async, [&, this, service]() {
    auto r =
        this->request("/service/register", Action::POST, service.to_json());
    entities::Service new_service;
    if (r.data.contains("service")) {
      new_service = entities::Service(r.data.at("service"));
    }
    Response<ServiceResponse> response = {r.status, r.success, r.message,
                                          ServiceResponse(new_service)};
    return response;
  });
}

Future<ServiceResponse> Client::service_update(entities::Service service) {
  return std::async(std::launch::async, [&, this, service]() {
    auto r = this->request("/service/" + service.id, Action::PATCH,
                           service.to_json());
    entities::Service updated_service;
    if (r.data.contains("service")) {
      updated_service = entities::Service(r.data.at("service"));
    }
    Response<ServiceResponse> response = {r.status, r.success, r.message,
                                          ServiceResponse(updated_service)};
    return response;
  });
}

Future<> Client::service_toggle_running(std::string id,
                                        types::ToggleRunningAction action) {
  return std::async(std::launch::async, [&, this, action]() {
    auto r = this->request("/service/" + id + "/running", Action::PATCH,
                           {{"action", types::to_string(action)}});
    Response<> response = {r.status, r.success, r.message, {}};
    return response;
  });
}

}  // namespace http
}  // namespace central
}  // namespace viacast
