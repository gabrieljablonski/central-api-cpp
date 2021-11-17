#pragma once

#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <future>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "CentralTypes.hpp"

namespace viacast::central::http {

typedef nlohmann::json Body;
typedef std::list<std::string> Headers;

template <typename DataType = Body>
struct Response {
  long status;
  bool success;
  std::string message;
  DataType data;
};

template <typename ResponseType = Body>
using PromisedResponse = std::future<Response<ResponseType>>;

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

struct DeviceResponse {};

struct DeviceKeygenResponse {
  std::string serial;
  std::string key;
};

class Client {
 private:
  std::string host;
  int port;
  std::string prefix;
  int32_t timeout;
  std::string locale;
  bool https;
  std::string token;
  std::string refresh_token;

  std::string build_url(std::string path);
  Headers build_headers();
  Response<Body> request(std::string path, Action action = Action::GET,
                         Body body = {});

 public:
  Client(std::string host, int port, std::string prefix = "/v1",
         int32_t timeout = 3L, std::string locale = "en", bool https = false);
  ~Client();

  PromisedResponse<AuthInfo> auth_login(std::string key, std::string password);
  PromisedResponse<> auth_request_verification_code();
  PromisedResponse<> auth_submit_verification_code(std::string code);
  PromisedResponse<AuthInfo> auth_refresh_token(std::string refresh_token);
  PromisedResponse<> auth_change_password(std::string old_password,
                                          std::string new_password);

  PromisedResponse<UserResponse> user_register(UserRegisterArgs user);
  PromisedResponse<UserResponse> user_me();
  PromisedResponse<DevicesResponse> user_my_devices();
  PromisedResponse<ServicesResponse> user_my_services();

  PromisedResponse<entities::Device> device_register(DeviceRegisterArgs device);
  PromisedResponse<std::list<entities::Service>> device_my_services();
  PromisedResponse<entities::Device> device_update_me(entities::Device device);
  PromisedResponse<entities::Device> device_update(entities::Device device);
  PromisedResponse<DeviceKeygenResponse> device_keygen(std::string serial,
                                                       bool force_new = false);
  PromisedResponse<> device_request_ownership(std::string serial);
  PromisedResponse<entities::Device> device_submit_ownership_code(
      std::string serial, std::string code, bool take_ownership = false);

  PromisedResponse<entities::Service> service_register(
      entities::Service service);
  PromisedResponse<entities::Service> service_update(entities::Service service);
  PromisedResponse<> service_toggle_running(std::string id,
                                            types::ToggleRunningAction action);
};

}  // namespace viacast::central::http
