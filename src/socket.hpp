#pragma once

#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "types.hpp"
#include "utils.hpp"

namespace viacast {
namespace central {
namespace socket {

enum class Event {
  TEST_EVENT,
  UPDATE_DEVICE_STATUS,
  DEVICE_UPDATED,
  DEVICE_STATUS_UPDATED,
  UPDATE_SERVICE_STATUS,
  SERVICE_UPDATED,
  SERVICE_STATUS_UPDATED,
  DEVICE_REQUEST_OWNERSHIP,
  SERVICE_TOGGLE_RUNNING
};
std::string event_to_string(Event event);

sio::message::ptr json_to_message(nlohmann::json json);
nlohmann::json message_to_json(sio::message::ptr message);

template <typename TData = nlohmann::json>
struct Response {
  bool success;
  std::string message;
  TData data;

  Response(bool success, std::string message);
  Response(bool success, std::string message, TData data);
};

template <typename... TArgs>
using Callback = std::function<void(TArgs... args)>;

struct EventHandler {
  Event event;
  Callback<nlohmann::json> handler;
};

template <typename TResponse = nlohmann::json>
using Future = std::future<Response<TResponse>>;

struct DeviceOnRequestOwnershipCallbackArgs {
  std::string code;
  uint64_t expiration;
};

struct ServiceOnToggleRunningCallbackArgs {
  std::string id;
  types::ToggleRunningAction action;
};

struct NotConnectedException : public std::exception {
  const char* what() const throw() { return "Socket not connected"; }
};

class Client {
 private:
  int port;
  std::string host;
  std::string path;
  int timeout;
  std::string locale;
  bool https;
  std::string token;
  std::list<EventHandler> event_handlers;
  sio::client client;

  std::string build_url();
  std::map<std::string, std::string> build_query();
  sio::message::ptr build_auth();
  Future<nlohmann::json> emit(Event event, nlohmann::json data);
  void on(Event event, Callback<nlohmann::json> handler);

 public:
  Client();
  Client(int port, std::string host = "localhost",
         std::string path = "/socket.io/", int timeout = 3000,
         std::string locale = "en", bool https = false, std::string token = "");

  void set_locale(std::string locale);
  void set_token(std::string token);
  bool connected() const;
  std::future<std::string> connect();
  std::future<std::string> try_connect();
  void disconnect();

  Future<> device_update_status(entities::DeviceStatus status);
  void device_on_update(Callback<entities::Device> callback);
  void device_on_update_status(Callback<entities::DeviceStatus> callback);
  void device_on_request_ownership(
      Callback<DeviceOnRequestOwnershipCallbackArgs> callback);

  Future<> service_update_status(entities::ServiceStatus status);
  void service_on_update(Callback<entities::Service> callback);
  void service_on_update_status(Callback<entities::ServiceStatus> callback);
  void service_on_toggle_running(
      Callback<ServiceOnToggleRunningCallbackArgs> callback);
};

}  // namespace socket
}  // namespace central
}  // namespace viacast
