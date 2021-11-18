#include "socket.hpp"

namespace viacast::central::socket {

std::string event_to_string(Event event) {
  std::map<Event, std::string> names = {
      {Event::TEST_EVENT, "test-event"},
      {Event::UPDATE_DEVICE_STATUS, "update-device-status"},
      {Event::DEVICE_UPDATED, "device-updated"},
      {Event::DEVICE_STATUS_UPDATED, "device-status-updated"},
      {Event::UPDATE_SERVICE_STATUS, "update-service-status"},
      {Event::SERVICE_UPDATED, "service-updated"},
      {Event::SERVICE_STATUS_UPDATED, "service-status-updated"},
      {Event::DEVICE_REQUEST_OWNERSHIP, "device-request-ownership"},
      {Event::SERVICE_TOGGLE_RUNNING, "service-toggle-running"}};
  return names.at(event);
}

sio::message::ptr json_to_message(nlohmann::json json) {
  if (json.is_boolean()) {
    return sio::bool_message::create(json.get<bool>());
  }
  if (json.is_number_integer()) {
    return sio::int_message::create(json.get<int64_t>());
  }
  if (json.is_number_float()) {
    return sio::double_message::create(json.get<double>());
  }
  if (json.is_string()) {
    return sio::string_message::create(json.get<std::string>());
  }
  if (json.is_array()) {
    sio::message::ptr m = sio::array_message::create();
    for (auto el : json) {
      m->get_vector().push_back(json_to_message(el));
    }
    return m;
  }
  if (json.is_object()) {
    sio::message::ptr m = sio::object_message::create();
    for (auto &j : json.items()) {
      m->get_map()[j.key()] = json_to_message(j.value());
    }
    return m;
  }
  return sio::null_message::create();
}

std::string Client::build_url() {
  return "http" + std::string(this->https ? "s" : "") + "://" + this->host +
         ":" + std::to_string(this->port) + this->path;
}

Future<nlohmann::json> Client::emit(Event event, nlohmann::json data) {
  return std::async(std::launch::async, [&, this, event, data]() {
    auto s = this->client.socket();
    s->emit(event_to_string(event), json_to_message(data));
    return nlohmann::json({});
  });
}

// void Client::on(Event event, Callback<nlohmann::json> handler) {}

Client::Client(int port, std::string host, std::string path, int timeout,
               std::string locale, bool https, std::string token)
    : port(port),
      host(host),
      path(path),
      timeout(timeout),
      locale(locale),
      https(https),
      token(token) {}

void Client::set_locale(std::string locale) { this->locale = locale; }

void Client::set_token(std::string token) { this->token = token; }

bool Client::connected() { return this->client.opened(); }

void Client::connect(Callback<> on_connect, Callback<> on_fail,
                     Callback<sio::client::close_reason> on_close) {
  this->disconnect();
  std::map<std::string, std::string> query = {
      {"token", this->token},
      {"locale", this->locale},
      {"timeout", std::to_string(this->timeout)}};
  this->client.set_open_listener(on_connect);
  this->client.set_fail_listener(on_fail);
  this->client.set_close_listener(on_close);
  this->client.set_socket_open_listener(
      [](std::string nsp) { std::cout << "socket open " << nsp << std::endl; });
  this->client.set_socket_close_listener([](std::string nsp) {
    std::cout << "socket close " << nsp << std::endl;
  });
  this->client.connect(this->build_url(), query);
}

// void Client::try_connect(Callback<> on_connect, Callback<> on_fail) {}

void Client::disconnect() { this->client.close(); }

// Future<> Client::device_update_status(entities::DeviceStatus
// status) {
// }

// void Client::device_on_update(Callback<entities::Device> callback) {}

// void Client::device_on_update_status(
//     Callback<entities::DeviceStatus> callback) {}

// void Client::device_on_request_ownership(
//     Callback<DeviceOnRequestOwnershipCallbackArgs> callback) {}

// Future<> Client::service_update_status(
//     entities::ServiceStatus status) {}

// void Client::service_on_update(Callback<entities::Service> callback) {}

// void Client::service_on_update_status(
//     Callback<entities::ServiceStatus> callback) {}

// void Client::service_on_toggle_running(
//     Callback<ServiceOnToggleRunningCallbackArgs> callback) {}

}  // namespace viacast::central::socket
