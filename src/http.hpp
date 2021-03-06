#pragma once

#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <future>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "types.hpp"

namespace viacast {
namespace central {
namespace http {

typedef std::list<std::string> Headers;

template <typename TData = nlohmann::json>
struct Response {
  long status;
  bool success;
  std::string message;
  TData data;
};

template <typename TResponse = nlohmann::json>
using Future = std::future<Response<TResponse>>;

enum class Action { GET, POST, PUT, PATCH, DELETE };
std::string action_to_string(Action action);

struct AuthInfo {
  bool verified;
  std::string token;
  std::string refresh_token;

  AuthInfo();
  AuthInfo(nlohmann::json json);
};

struct UserRegisterArgs {
  std::string email;
  std::string name;
  std::string organization;
  std::string password;
  std::string captcha_token;

  nlohmann::json to_json() const;
};

struct UserResponse {
  entities::User user;

  UserResponse(entities::User user);
};

struct DevicesResponse {
  std::list<entities::Device> devices;

  DevicesResponse(std::list<entities::Device> devices);
};

struct ServicesResponse {
  std::list<entities::Service> services;

  ServicesResponse(std::list<entities::Service> services);
};

struct DeviceRegisterArgs {
  std::string serial;
  std::string display_name;
  std::string model;
  std::string key;

  nlohmann::json to_json() const;
};

struct DeviceResponse {
  entities::Device device;

  DeviceResponse(entities::Device device);
};

struct DeviceKeygenResponse {
  std::string serial;
  std::string key;

  DeviceKeygenResponse();
  DeviceKeygenResponse(std::string serial, std::string key);
};

struct ServiceResponse {
  entities::Service service;

  ServiceResponse(entities::Service service);
};

class Client {
 private:
  int port;
  std::string host;
  std::string prefix;
  int32_t timeout;
  std::string locale;
  bool https;
  std::string token;
  std::string refresh_token;
  bool verbose;

  std::string build_url(std::string path);
  Headers build_headers();
  Response<nlohmann::json> request(std::string path,
                                   Action action = Action::GET,
                                   nlohmann::json body = {});

 public:
  Client();
  Client(int port, std::string host = "localhost", std::string prefix = "/v1",
         int32_t timeout = 3L, std::string locale = "en", bool https = false,
         bool verbose = false);
  ~Client();

  Future<AuthInfo> auth_login(std::string key, std::string password);
  Future<> auth_request_verification_code();
  Future<> auth_submit_verification_code(std::string code);
  Future<AuthInfo> auth_refresh_token(std::string refresh_token);
  Future<> auth_change_password(std::string old_password,
                                std::string new_password);

  Future<UserResponse> user_register(UserRegisterArgs user);
  Future<UserResponse> user_me();
  Future<DevicesResponse> user_my_devices();
  Future<ServicesResponse> user_my_services();

  Future<DeviceResponse> device_register(DeviceRegisterArgs device);
  Future<DeviceResponse> device_me();
  Future<ServicesResponse> device_my_services();
  Future<DeviceResponse> device_update_me(entities::Device device);
  Future<DeviceResponse> device_update(entities::Device device);
  Future<DeviceKeygenResponse> device_keygen(std::string serial,
                                             bool force_new = false);
  Future<> device_request_ownership(std::string serial);
  Future<DeviceResponse> device_submit_ownership_code(
      std::string serial, std::string code, bool take_ownership = false);

  Future<ServiceResponse> service_register(entities::Service service);
  Future<ServiceResponse> service_update(entities::Service service);
  Future<> service_toggle_running(std::string id,
                                  types::ToggleRunningAction action);
};

}  // namespace http
}  // namespace central
}  // namespace viacast
