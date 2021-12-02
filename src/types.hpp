#pragma once

#include <list>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace viacast {
namespace central {
namespace types {

enum class AuthRole { null, ADMIN, KEYGEN, USER, DEVICE };
enum class ServiceType { null, FEED, OUTPUT };
enum class ServiceStatusType { null, OFF, READY, LOAD, LIVE };
enum class DeviceStatusType { null, OFF, ON, REBOOTING };
enum class LeaseType { null, EXCLUSIVE, NON_EXCLUSIVE };
enum class ToggleRunningAction { null, START, STOP, RESTART };

void from_string(std::string str, AuthRole *role);
void from_string(std::string str, ServiceType *type);
void from_string(std::string str, ServiceStatusType *type);
void from_string(std::string str, DeviceStatusType *type);
void from_string(std::string str, LeaseType *type);
void from_string(std::string str, ToggleRunningAction *action);

std::string to_string(AuthRole role);
std::string to_string(ServiceType type);
std::string to_string(ServiceStatusType type);
std::string to_string(DeviceStatusType type);
std::string to_string(LeaseType type);
std::string to_string(ToggleRunningAction action);

}  // namespace types

namespace entities {

struct Auth {
  std::string id;
  std::string key;
  viacast::central::types::AuthRole role;

  Auth();
  Auth(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct Group {
  std::string id;
  std::string display_name;

  Group();
  Group(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct ServiceOperationMode {
  std::string id;
  std::string name;
  std::string display_name;
  viacast::central::types::ServiceType type;
  std::string config_layout;

  ServiceOperationMode();
  ServiceOperationMode(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct Service {
  std::string id;
  std::string name;
  std::string display_name;
  std::string config;
  ServiceOperationMode operation_mode;
  std::string operation_mode_id;
  std::list<std::string> linked_service_ids;
  std::string device_id;

  Service();
  Service(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct ServiceStatus {
  std::string id;
  viacast::central::types::ServiceStatusType status;
  nlohmann::json info;

  ServiceStatus();
  ServiceStatus(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct ServiceWithStatus : Service, ServiceStatus {
  std::string id;

  ServiceWithStatus();
  ServiceWithStatus(nlohmann::json json);
  nlohmann::json to_json() const;
  void merge(ServiceWithStatus service);

  void operator=(Service const &d);
};

struct Device {
  std::string id;
  std::string display_name;
  std::string serial;
  std::string user_id;
  std::string model;
  Group group;
  std::list<std::string> service_ids;
  std::list<std::string> lease_ids;
  std::list<ServiceOperationMode> service_operation_modes;

  Device();
  Device(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct DeviceStatus {
  std::string id;
  viacast::central::types::DeviceStatusType status;
  nlohmann::json info;

  DeviceStatus();
  DeviceStatus(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct DeviceWithStatus : Device, DeviceStatus {
  std::string id;

  DeviceWithStatus();
  DeviceWithStatus(nlohmann::json json);
  nlohmann::json to_json() const;
  void merge(Device device);
  void merge(DeviceWithStatus device);

  void operator=(Device const &s);
};

struct Lease {
  std::string id;
  std::string annotations;
  viacast::central::types::LeaseType type;
  uint64_t start;
  uint64_t expiration;
  bool canceled;
  std::string owner_id;
  std::string borrower_id;
  std::list<std::string> device_ids;

  Lease();
  Lease(nlohmann::json json);
  nlohmann::json to_json() const;
};

struct User {
  std::string id;
  std::string email;
  std::string name;
  std::string organization;
  Group group;
  std::list<std::string> device_ids;
  std::list<std::string> leases_owned_ids;
  std::list<std::string> leases_borrowed_ids;

  User();
  User(nlohmann::json json);
  nlohmann::json to_json() const;
};

}  // namespace entities

}  // namespace central
}  // namespace viacast
