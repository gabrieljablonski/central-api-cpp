#include "central-api.hpp"

namespace viacast::central {

CentralApi::CentralApi(int port, std::string host, std::string prefix,
                       int timeout, std::string locale, bool https)
    : port(port),
      host(host),
      prefix(prefix),
      timeout(timeout),
      locale(locale),
      https(https) {
  this->http = new http::Client(port, host, prefix, timeout, locale, https);
  this->socket = new socket::Client(port, host, this->prefix + "/socket.io/",
                                    timeout, locale, https);
}

http::Future<http::AuthInfo> CentralApi::auth_login(std::string key,
                                                    std::string password) {
  return std::async(std::launch::async, [&, this, key, password] {
    auto r = this->http->auth_login(key, password).get();
    if (r.success && !r.data.token.empty()) {
      this->socket->set_token(r.data.token);
    }
    return r;
  });
}

http::Future<> CentralApi::auth_request_verification_code() {
  return this->http->auth_request_verification_code();
}

http::Future<> CentralApi::auth_submit_verification_code(std::string code) {
  return this->http->auth_submit_verification_code(code);
}

http::Future<http::AuthInfo> CentralApi::auth_refresh_token(
    std::string refresh_token) {
  return std::async(std::launch::async, [&, this, refresh_token] {
    auto r = this->http->auth_refresh_token(refresh_token).get();
    if (r.success && !r.data.token.empty()) {
      this->socket->set_token(r.data.token);
    }
    return r;
  });
}

http::Future<> CentralApi::auth_change_password(std::string old_password,
                                                std::string new_password) {
  return this->http->auth_change_password(old_password, new_password);
}

http::Future<http::UserResponse> CentralApi::user_register(
    http::UserRegisterArgs user) {
  return this->http->user_register(user);
}

http::Future<http::UserResponse> CentralApi::user_me() {
  return this->http->user_me();
}

http::Future<http::DevicesResponse> CentralApi::user_my_devices() {
  return this->http->user_my_devices();
}

http::Future<http::ServicesResponse> CentralApi::user_my_services() {
  return this->http->user_my_services();
}

http::Future<http::DeviceResponse> CentralApi::device_register(
    http::DeviceRegisterArgs device) {
  return this->http->device_register(device);
}

http::Future<http::ServicesResponse> CentralApi::device_my_services() {
  return this->http->device_my_services();
}

http::Future<http::DeviceResponse> CentralApi::device_update_me(
    entities::Device device) {
  return this->http->device_update_me(device);
}

http::Future<http::DeviceResponse> CentralApi::device_update(
    entities::Device device) {
  return this->http->device_update(device);
}

http::Future<http::DeviceKeygenResponse> CentralApi::device_keygen(
    std::string serial, bool force_new) {
  return this->http->device_keygen(serial, force_new);
}

http::Future<> CentralApi::device_request_ownership(std::string serial) {
  return this->http->device_request_ownership(serial);
}

http::Future<http::DeviceResponse> CentralApi::device_submit_ownership_code(
    std::string serial, std::string code, bool take_ownership) {
  return this->http->device_submit_ownership_code(serial, code, take_ownership);
}

http::Future<http::ServiceResponse> CentralApi::service_register(
    entities::Service service) {
  return this->http->service_register(service);
}

http::Future<http::ServiceResponse> CentralApi::service_update(
    entities::Service service) {
  return this->http->service_update(service);
}

http::Future<> CentralApi::service_toggle_running(
    std::string id, types::ToggleRunningAction action) {
  return this->http->service_toggle_running(id, action);
}

bool CentralApi::socket_connected() { return this->socket->connected(); }

std::future<std::string> CentralApi::socket_connect() {
  return this->socket->connect();
}

std::future<std::string> CentralApi::socket_try_connect() {
  return this->socket->try_connect();
}

void CentralApi::socket_disconnect() { return this->socket->disconnect(); }

socket::Future<> CentralApi::device_update_status(
    entities::DeviceStatus status) {
  return this->socket->device_update_status(status);
}

void CentralApi::device_on_update(socket::Callback<entities::Device> callback) {
  return this->socket->device_on_update(callback);
}

void CentralApi::device_on_update_status(
    socket::Callback<entities::DeviceStatus> callback) {
  return this->socket->device_on_update_status(callback);
}

void CentralApi::device_on_request_ownership(
    socket::Callback<socket::DeviceOnRequestOwnershipCallbackArgs> callback) {
  return this->socket->device_on_request_ownership(callback);
}

socket::Future<> CentralApi::service_update_status(
    entities::ServiceStatus status) {
  return this->socket->service_update_status(status);
}

void CentralApi::service_on_update(
    socket::Callback<entities::Service> callback) {
  return this->socket->service_on_update(callback);
}

void CentralApi::service_on_update_status(
    socket::Callback<entities::ServiceStatus> callback) {
  return this->socket->service_on_update_status(callback);
}

void CentralApi::service_on_toggle_running(
    socket::Callback<socket::ServiceOnToggleRunningCallbackArgs> callback) {
  return this->socket->service_on_toggle_running(callback);
}

}  // namespace viacast::central
