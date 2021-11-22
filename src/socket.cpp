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

template <typename TData>
Response<TData>::Response(bool success, std::string message)
    : success(success), message(message) {}

template <typename TData>
Response<TData>::Response(bool success, std::string message, TData data)
    : success(success), message(message), data(data) {}

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
  auto p = std::make_shared<std::promise<Response<nlohmann::json>>>();
  std::cerr << "emitting " << event_to_string(event) << " " << data.dump()
            << std::endl;
  s->emit(event_to_string(event), json_to_message(data),
          [p](sio::message::list const& args) {
            Response<> r(false, "event failed");
            if (!args.size()) {
              p->set_value(r);
              return;
            }
            auto j = message_to_json(args.at(0));
            if (j.contains("success")) {
              j.at("success").get_to(r.success);
            }
            if (j.contains("message")) {
              j.at("message").get_to(r.message);
            }
            if (j.contains("data")) {
              r.data = j.at("data");
            }
            p->set_value(r);
            return;
          });
  return p->get_future();
}

void Client::on(Event event, Callback<nlohmann::json> handler) {
  this->client.socket()->on(event_to_string(event), [&, handler](sio::event e) {
    auto messages = e.get_messages();
    if (!messages.size()) {
      handler(nlohmann::json());
      return;
    }
    auto j = message_to_json(messages.at(0));
    handler(j);
  });
}

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

std::future<std::string> Client::connect() {
  auto p = std::make_shared<std::promise<std::string>>();

  this->disconnect();

  this->client.set_logs_quiet();

  this->client.set_socket_open_listener([p](std::string) {
    std::cerr << "socket open" << std::endl;
    try {
      p->set_value("");
    } catch (std::future_error const& e) {
    }
  });
  this->client.set_socket_close_listener(
      [&, this](std::string) { std::cerr << "socket close" << std::endl; });

  this->client.connect(this->build_url(), this->build_query(),
                       this->build_auth());
  this->client.socket()->on_error([&, this, p](sio::message::ptr message) {
    std::string m = "unknown reason";
    if (message->get_flag() == sio::message::flag_object &&
        message->get_map().count("message")) {
      m = message->get_map().at("message")->get_string();
      this->disconnect();
    }
    // std::cerr << "socket error: '" << m << "'" << std::endl;
    try {
      p->set_value(m);
    } catch (std::future_error const& e) {
    }
  });

  return p->get_future();
}

std::future<std::string> Client::try_connect() {
  if (!this->connected()) {
    return this->connect();
  }
  std::promise<std::string> p;
  p.set_value("");
  return p.get_future();
}

void Client::disconnect() { this->client.close(); }

Future<> Client::device_update_status(entities::DeviceStatus status) {
  return this->emit(Event::UPDATE_DEVICE_STATUS,
                    {{"id", status.id},
                     {"status", types::to_string(status.status)},
                     {"info", status.info}});
}

void Client::device_on_update(Callback<entities::Device> callback) {
  return this->on(Event::DEVICE_UPDATED, [&, callback](nlohmann::json json) {
    if (json.contains("device")) {
      callback(entities::Device(json.at("device")));
      return;
    }
    callback(nlohmann::json());
  });
}

void Client::device_on_update_status(
    Callback<entities::DeviceStatus> callback) {
  return this->on(Event::DEVICE_STATUS_UPDATED,
                  [&, callback](nlohmann::json json) {
                    if (json.contains("status")) {
                      callback(entities::DeviceStatus(json.at("status")));
                      return;
                    }
                    callback(nlohmann::json());
                  });
}

void Client::device_on_request_ownership(
    Callback<DeviceOnRequestOwnershipCallbackArgs> callback) {
  return this->on(
      Event::DEVICE_REQUEST_OWNERSHIP, [&, callback](nlohmann::json json) {
        DeviceOnRequestOwnershipCallbackArgs args;
        if (json.contains("code")) {
          if (json.at("code").contains("code")) {
            json.at("code").at("code").get_to(args.code);
          }
          if (json.at("code").contains("expiration")) {
            json.at("code").at("expiration").get_to(args.expiration);
          }
        }
        callback(args);
      });
}

Future<> Client::service_update_status(entities::ServiceStatus status) {
  return this->emit(Event::UPDATE_SERVICE_STATUS,
                    {{"id", status.id},
                     {"status", types::to_string(status.status)},
                     {"info", status.info}});
}

void Client::service_on_update(Callback<entities::Service> callback) {
  return this->on(Event::SERVICE_UPDATED, [&, callback](nlohmann::json json) {
    if (json.contains("service")) {
      callback(entities::Service(json.at("service")));
      return;
    }
    callback(nlohmann::json());
  });
}

void Client::service_on_update_status(
    Callback<entities::ServiceStatus> callback) {
  return this->on(Event::SERVICE_STATUS_UPDATED,
                  [&, callback](nlohmann::json json) {
                    if (json.contains("status")) {
                      callback(entities::ServiceStatus(json.at("status")));
                      return;
                    }
                    callback(nlohmann::json());
                  });
}

void Client::service_on_toggle_running(
    Callback<ServiceOnToggleRunningCallbackArgs> callback) {
  return this->on(Event::SERVICE_TOGGLE_RUNNING,
                  [&, callback](nlohmann::json json) {
                    ServiceOnToggleRunningCallbackArgs args;
                    if (json.contains("action")) {
                      if (json.at("action").contains("id")) {
                        json.at("action").at("id").get_to(args.id);
                      }
                      if (json.at("action").contains("action")) {
                        std::string action;
                        json.at("action").at("action").get_to(action);
                        types::from_string(action, &args.action);
                      }
                    }
                    callback(args);
                  });
}

}  // namespace viacast::central::socket
