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
    for (auto& j : json.items()) {
      m->get_map()[j.key()] = json_to_message(j.value());
    }
    return m;
  }
  return sio::null_message::create();
}

nlohmann::json message_to_json(sio::message::ptr message) {
  switch (message->get_flag()) {
    case sio::message::flag_integer:
      return nlohmann::json(message->get_int());
    case sio::message::flag_double:
      return nlohmann::json(message->get_double());
    case sio::message::flag_string:
      return nlohmann::json(message->get_string());
    case sio::message::flag_boolean:
      return nlohmann::json(message->get_bool());
    case sio::message::flag_array: {
      nlohmann::json j;
      for (auto& m : message->get_vector()) {
        j.push_back(message_to_json(m));
      }
      return j;
    }
    case sio::message::flag_object: {
      nlohmann::json j;
      for (auto& m : message->get_map()) {
        j[m.first] = message_to_json(m.second);
      }
      return j;
    }
    default:
      return nlohmann::json();
  }
  return nlohmann::json();
}

std::string Client::build_url() {
  return "http" + std::string(this->https ? "s" : "") + "://" + this->host +
         ":" + std::to_string(this->port) + this->path;
}

std::map<std::string, std::string> Client::build_query() {
  std::map<std::string, std::string> query = {
      {"locale", this->locale}, {"timeout", std::to_string(this->timeout)}};
  return query;
}

sio::message::ptr Client::build_auth() {
  sio::message::ptr auth = sio::object_message::create();
  auth->get_map()["token"] = sio::string_message::create(this->token);
  return auth;
}

Future<nlohmann::json> Client::emit(Event event, nlohmann::json data) {
  if (!this->connected()) {
    std::cerr << "socket not connected" << std::endl;
    throw NotConnectedException();
  }
  auto s = this->client.socket();
  auto p = std::make_shared<std::promise<nlohmann::json>>();
  std::cerr << "emitting " << event_to_string(event) << " " << data.dump()
            << std::endl;
  s->emit(event_to_string(event), json_to_message(data),
          [p](sio::message::list const& args) {
            if (!args.size()) {
              p->set_value(nlohmann::json({}));
              return;
            }
            p->set_value(message_to_json(args.at(0)));
            return;
          });
  return p->get_future();
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

bool Client::connected() const { return this->client.opened(); }

void Client::connect(Callback<Client*> on_connect,
                     Callback<std::string> on_fail, Callback<> on_close) {
  this->disconnect();

  this->client.set_logs_verbose();

  this->client.set_socket_open_listener(
      [&, this, on_connect](std::string) { on_connect(this); });
  this->client.set_socket_close_listener(
      [on_close](std::string) { on_close(); });

  this->client.connect(this->build_url(), this->build_query(),
                       this->build_auth());
  this->client.socket()->on_error(
      [&, this, on_fail](sio::message::ptr message) {
        if (message->get_flag() == sio::message::flag_object &&
            message->get_map().count("message")) {
          std::string m = message->get_map().at("message")->get_string();
          on_fail(m);
          this->disconnect();
        }
        on_fail("unknown reason");
      });
}

void Client::try_connect(Callback<Client*> on_connect,
                         Callback<std::string> on_fail, Callback<> on_close) {
  if (!this->connected()) {
    this->connect(on_connect, on_fail, on_close);
  }
}

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
