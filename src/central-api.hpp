#pragma once

#include <future>
#include <string>

#include "http.hpp"
#include "socket.hpp"

namespace viacast {
namespace central {

class CentralApi {
 private:
  int port;
  std::string host;
  std::string prefix;
  int32_t timeout;
  std::string locale;
  bool https;
  bool verbose;

  http::Client *http;
  socket::Client *socket;

 public:
  CentralApi(int port = 4440, std::string host = "localhost",
             std::string prefix = "/v1", int timeout = 3000,
             std::string locale = "en", bool https = false,
             bool verbose = false);

  http::Future<http::AuthInfo> auth_login(std::string key,
                                          std::string password);
  http::Future<> auth_request_verification_code();
  http::Future<> auth_submit_verification_code(std::string code);
  http::Future<http::AuthInfo> auth_refresh_token(std::string refresh_token);
  http::Future<> auth_change_password(std::string old_password,
                                      std::string new_password);
  http::Future<http::UserResponse> user_register(http::UserRegisterArgs user);
  http::Future<http::UserResponse> user_me();
  http::Future<http::DevicesResponse> user_my_devices();
  http::Future<http::ServicesResponse> user_my_services();
  http::Future<http::DeviceResponse> device_register(
      http::DeviceRegisterArgs device);
  http::Future<http::DeviceResponse> device_me();
  http::Future<http::ServicesResponse> device_my_services();
  http::Future<http::DeviceResponse> device_update_me(entities::Device device);
  http::Future<http::DeviceResponse> device_update(entities::Device device);
  http::Future<http::DeviceKeygenResponse> device_keygen(
      std::string serial, bool force_new = false);
  http::Future<> device_request_ownership(std::string serial);
  http::Future<http::DeviceResponse> device_submit_ownership_code(
      std::string serial, std::string code, bool take_ownership = false);
  http::Future<http::ServiceResponse> service_register(
      entities::Service service);
  http::Future<http::ServiceResponse> service_update(entities::Service service);
  http::Future<> service_toggle_running(std::string id,
                                        types::ToggleRunningAction action);

  bool socket_connected();
  std::future<std::string> socket_connect();
  void socket_try_connect();
  void socket_disconnect();
  socket::Future<> device_update_status(entities::DeviceStatus status);
  void device_on_update(socket::Callback<entities::Device> callback);
  void device_on_update_status(
      socket::Callback<entities::DeviceStatus> callback);
  void device_on_request_ownership(
      socket::Callback<socket::DeviceOnRequestOwnershipCallbackArgs> callback);
  socket::Future<> service_update_status(entities::ServiceStatus status);
  void service_on_update(socket::Callback<entities::Service> callback);
  void service_on_update_status(
      socket::Callback<entities::ServiceStatus> callback);
  void service_on_toggle_running(
      socket::Callback<socket::ServiceOnToggleRunningCallbackArgs> callback);
};

}  // namespace central
}  // namespace viacast