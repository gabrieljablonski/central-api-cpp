#include "types.hpp"

namespace viacast::central::types {

void from_string(std::string str, AuthRole *role) {
  std::map<std::string, AuthRole> types = {{"ADMIN", AuthRole::ADMIN},
                                           {"KEYGEN", AuthRole::KEYGEN},
                                           {"USER", AuthRole::USER},
                                           {"DEVICE", AuthRole::DEVICE}};
  try {
    *role = types.at(str);
  } catch (std::out_of_range const &e) {
    *role = AuthRole::null;
  }
}

void from_string(std::string str, ServiceType *type) {
  std::map<std::string, ServiceType> types = {{"FEED", ServiceType::FEED},
                                              {"OUTPUT", ServiceType::OUTPUT}};
  try {
    *type = types.at(str);
  } catch (std::out_of_range const &e) {
    *type = ServiceType::null;
  }
}

void from_string(std::string str, ServiceStatusType *type) {
  std::map<std::string, ServiceStatusType> types = {
      {"OFF", ServiceStatusType::OFF},
      {"READY", ServiceStatusType::READY},
      {"LOAD", ServiceStatusType::LOAD},
      {"LIVE", ServiceStatusType::LIVE}};
  try {
    *type = types.at(str);
  } catch (std::out_of_range const &e) {
    *type = ServiceStatusType::null;
  }
}

void from_string(std::string str, DeviceStatusType *type) {
  std::map<std::string, DeviceStatusType> types = {
      {"OFF", DeviceStatusType::OFF},
      {"ON", DeviceStatusType::ON},
      {"REBOOTING", DeviceStatusType::REBOOTING}};
  try {
    *type = types.at(str);
  } catch (std::out_of_range const &e) {
    *type = DeviceStatusType::null;
  }
}

void from_string(std::string str, LeaseType *type) {
  std::map<std::string, LeaseType> types = {
      {"EXCLUSIVE", LeaseType::EXCLUSIVE},
      {"NON_EXCLUSIVE", LeaseType::NON_EXCLUSIVE}};
  try {
    *type = types.at(str);
  } catch (std::out_of_range const &e) {
    *type = LeaseType::null;
  }
}

void from_string(std::string str, ToggleRunningAction *action) {
  std::map<std::string, ToggleRunningAction> types = {
      {"start", ToggleRunningAction::START},
      {"stop", ToggleRunningAction::STOP},
      {"restart", ToggleRunningAction::RESTART}};
  try {
    *action = types.at(str);
  } catch (std::out_of_range const &e) {
    *action = ToggleRunningAction::null;
  }
}

std::string to_string(AuthRole role) {
  std::map<AuthRole, std::string> names = {{AuthRole::ADMIN, "ADMIN"},
                                           {AuthRole::KEYGEN, "KEYGEN"},
                                           {AuthRole::USER, "USER"},
                                           {AuthRole::DEVICE, "DEVICE"}};
  return names.at(role);
}

std::string to_string(ServiceType type) {
  std::map<ServiceType, std::string> names = {{ServiceType::FEED, "FEED"},
                                              {ServiceType::OUTPUT, "OUTPUT"}};
  return names.at(type);
}

std::string to_string(ServiceStatusType type) {
  std::map<ServiceStatusType, std::string> names = {
      {ServiceStatusType::OFF, "OFF"},
      {ServiceStatusType::READY, "READY"},
      {ServiceStatusType::LOAD, "LOAD"},
      {ServiceStatusType::LIVE, "LIVE"}};
  return names.at(type);
}

std::string to_string(DeviceStatusType type) {
  std::map<DeviceStatusType, std::string> names = {
      {DeviceStatusType::OFF, "OFF"},
      {DeviceStatusType::ON, "ON"},
      {DeviceStatusType::REBOOTING, "REBOOTING"}};
  return names.at(type);
}

std::string to_string(LeaseType type) {
  std::map<LeaseType, std::string> names = {
      {LeaseType::EXCLUSIVE, "EXCLUSIVE"},
      {LeaseType::NON_EXCLUSIVE, "NON_EXCLUSIVE"}};
  return names.at(type);
}

std::string to_string(ToggleRunningAction action) {
  std::map<ToggleRunningAction, std::string> names = {
      {ToggleRunningAction::START, "start"},
      {ToggleRunningAction::STOP, "stop"},
      {ToggleRunningAction::RESTART, "restart"}};
  return names.at(action);
}

}  // namespace viacast::central::types

namespace viacast::central::entities {

Auth::Auth() {}

Auth::Auth(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("key")) {
    json.at("key").get_to(this->key);
  }
  if (json.contains("role")) {
    viacast::central::types::from_string(json.at("role").get<std::string>(),
                                         &this->role);
  }
}

nlohmann::json Auth::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->key.empty()) {
    j["key"] = this->key;
  }
  if (this->role != viacast::central::types::AuthRole::null) {
    j["role"] = viacast::central::types::to_string(this->role);
  }
  return j;
}

Group::Group() {}

Group::Group(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("displayName")) {
    json.at("displayName").get_to(this->display_name);
  }
}

nlohmann::json Group::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  return j;
}

ServiceOperationMode::ServiceOperationMode() {}

ServiceOperationMode::ServiceOperationMode(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("name")) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("displayName")) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("type")) {
    viacast::central::types::from_string(json.at("type").get<std::string>(),
                                         &this->type);
  }
  if (json.contains("configLayout")) {
    json.at("configLayout").get_to(this->config_layout);
  }
}

nlohmann::json ServiceOperationMode::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->name.empty()) {
    j["name"] = this->name;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  if (this->type != viacast::central::types::ServiceType::null) {
    j["type"] = viacast::central::types::to_string(this->type);
  }
  if (!this->config_layout.empty()) {
    j["configLayout"] = this->config_layout;
  }
  return j;
}

Service::Service() {}

Service::Service(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("name")) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("config")) {
    json.at("config").get_to(this->config);
  }
  if (json.contains("operationMode")) {
    this->operation_mode = ServiceOperationMode(json.at("operationMode"));
  }
  if (json.contains("operationModeId")) {
    json.at("operationModeId").get_to(this->operation_mode_id);
  }
  if (json.contains("linkedServiceIds")) {
    json.at("linkedServiceIds").get_to(this->linked_service_ids);
  }
  if (json.contains("deviceId")) {
    json.at("deviceId").get_to(this->device_id);
  }
}

nlohmann::json Service::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->name.empty()) {
    j["name"] = this->name;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  if (!this->config.empty()) {
    j["config"] = this->config;
  }
  if (!this->operation_mode.name.empty()) {
    j["operationMode"] = this->operation_mode.to_json();
  }
  if (!this->operation_mode_id.empty()) {
    j["operationModeId"] = this->operation_mode_id;
  }

  j["linkedServiceIds"] = this->linked_service_ids;

  if (!this->device_id.empty()) {
    j["deviceId"] = this->device_id;
  }
  return j;
}

ServiceStatus::ServiceStatus() {}

ServiceStatus::ServiceStatus(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("status")) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  }
  if (json.contains("info")) {
    json.at("info").get_to(this->info);
  }
}

nlohmann::json ServiceStatus::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (this->status != viacast::central::types::ServiceStatusType::null) {
    j["status"] = viacast::central::types::to_string(this->status);
  }
  if (!this->info.empty()) {
    j["info"] = this->info;
  }
  return j;
}

ServiceWithStatus::ServiceWithStatus() {}

ServiceWithStatus::ServiceWithStatus(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("name")) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("config")) {
    json.at("config").get_to(this->config);
  }
  if (json.contains("operationMode")) {
    this->operation_mode = ServiceOperationMode(json.at("operationMode"));
  }
  if (json.contains("operationModeId")) {
    json.at("operationModeId").get_to(this->operation_mode_id);
  }
  if (json.contains("linkedServiceIds")) {
    json.at("linkedServiceIds").get_to(this->linked_service_ids);
  }
  if (json.contains("deviceId")) {
    json.at("deviceId").get_to(this->device_id);
  }
  if (json.contains("status")) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  }
  if (json.contains("info")) {
    json.at("info").get_to(this->info);
  }
}

nlohmann::json ServiceWithStatus::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->name.empty()) {
    j["name"] = this->name;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  if (!this->config.empty()) {
    j["config"] = this->config;
  }
  if (!this->operation_mode.name.empty()) {
    j["operationMode"] = this->operation_mode.to_json();
  }
  if (!this->operation_mode_id.empty()) {
    j["operationModeId"] = this->operation_mode_id;
  }

  j["linkedServiceIds"] = this->linked_service_ids;

  if (!this->device_id.empty()) {
    j["deviceId"] = this->device_id;
  }
  if (this->status != viacast::central::types::ServiceStatusType::null) {
    j["status"] = viacast::central::types::to_string(this->status);
  }
  if (!this->info.empty()) {
    j["info"] = this->info;
  }
  return j;
}

Device::Device() {}

Device::Device(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("displayName")) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("serial")) {
    json.at("serial").get_to(this->serial);
  }
  if (json.contains("userId")) {
    json.at("userId").get_to(this->user_id);
  }
  if (json.contains("model")) {
    json.at("model").get_to(this->model);
  }
  if (json.contains("group")) {
    this->group = Group(json.at("group"));
  }
  if (json.contains("serviceIds")) {
    json.at("serviceIds").get_to(this->service_ids);
  }
  if (json.contains("leaseIds")) {
    json.at("leaseIds").get_to(this->lease_ids);
  }
  if (json.contains("serviceOperationModes")) {
    for (auto op : json.at("serviceOperationModes")) {
      this->service_operation_modes.push_back(ServiceOperationMode(op));
    }
  }
}

nlohmann::json Device::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  if (!this->serial.empty()) {
    j["serial"] = this->serial;
  }
  if (!this->user_id.empty()) {
    j["userId"] = this->user_id;
  }
  if (!this->model.empty()) {
    j["model"] = this->model;
  }
  if (!this->group.id.empty()) {
    j["group"] = this->group.to_json();
  }

  j["serviceIds"] = this->service_ids;

  j["leaseIds"] = this->lease_ids;

  nlohmann::json ops;
  for (auto op : this->service_operation_modes) {
    ops.push_back(op.to_json());
  }
  j["serviceOperationModes"] = ops;

  return j;
}

DeviceStatus::DeviceStatus() {}

DeviceStatus::DeviceStatus(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("status")) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  }
  if (json.contains("info")) {
    json.at("info").get_to(this->info);
  }
}

nlohmann::json DeviceStatus::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (this->status != viacast::central::types::DeviceStatusType::null) {
    j["status"] = viacast::central::types::to_string(this->status);
  }
  if (!this->info.empty()) {
    j["info"] = this->info;
  }
  return j;
}

DeviceWithStatus::DeviceWithStatus() {}

DeviceWithStatus::DeviceWithStatus(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("displayName")) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("serial")) {
    json.at("serial").get_to(this->serial);
  }
  if (json.contains("userId")) {
    json.at("userId").get_to(this->user_id);
  }
  if (json.contains("model")) {
    json.at("model").get_to(this->model);
  }
  if (json.contains("group")) {
    this->group = Group(json.at("group"));
  }
  if (json.contains("serviceIds")) {
    json.at("serviceIds").get_to(this->service_ids);
  }
  if (json.contains("leaseIds")) {
    json.at("leaseIds").get_to(this->lease_ids);
  }
  if (json.contains("serviceOperationModes")) {
    for (auto op : json.at("serviceOperationModes")) {
      this->service_operation_modes.push_back(ServiceOperationMode(op));
    }
  }
  if (json.contains("status")) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  }
  if (json.contains("info")) {
    json.at("info").get_to(this->info);
  }
}

nlohmann::json DeviceWithStatus::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->display_name.empty()) {
    j["displayName"] = this->display_name;
  }
  if (!this->serial.empty()) {
    j["serial"] = this->serial;
  }
  if (!this->user_id.empty()) {
    j["userId"] = this->user_id;
  }
  if (!this->model.empty()) {
    j["model"] = this->model;
  }
  if (!this->group.id.empty()) {
    j["group"] = this->group.to_json();
  }

  j["serviceIds"] = this->service_ids;

  j["leaseIds"] = this->lease_ids;

  nlohmann::json ops;
  for (auto op : this->service_operation_modes) {
    ops.push_back(op.to_json());
  }
  j["serviceOperationModes"] = ops;

  if (this->status != viacast::central::types::DeviceStatusType::null) {
    j["status"] = viacast::central::types::to_string(this->status);
  }
  if (!this->info.empty()) {
    j["info"] = this->info;
  }
  return j;
}

Lease::Lease() {}

Lease::Lease(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("annotations")) {
    json.at("annotations").get_to(this->annotations);
  }
  if (json.contains("type")) {
    viacast::central::types::from_string(json.at("type").get<std::string>(),
                                         &this->type);
  }
  if (json.contains("start")) {
    json.at("start").get_to(this->start);
  }
  if (json.contains("expiration")) {
    json.at("expiration").get_to(this->expiration);
  }
  if (json.contains("canceled")) {
    json.at("canceled").get_to(this->canceled);
  }
  if (json.contains("ownerId")) {
    json.at("ownerId").get_to(this->owner_id);
  }
  if (json.contains("borrowerId")) {
    json.at("borrowerId").get_to(this->borrower_id);
  }
  if (json.contains("deviceIds")) {
    json.at("deviceIds").get_to(this->device_ids);
  }
}

nlohmann::json Lease::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->annotations.empty()) {
    j["annotations"] = this->annotations;
  }
  if (this->type != viacast::central::types::LeaseType::null) {
    j["type"] = viacast::central::types::to_string(this->type);
  }
  if (!this->start) {
    j["start"] = this->start;
  }
  if (!this->expiration) {
    j["expiration"] = this->expiration;
  }

  j["canceled"] = this->canceled;

  if (!this->owner_id.empty()) {
    j["ownerId"] = this->owner_id;
  }
  if (!this->borrower_id.empty()) {
    j["borrowerId"] = this->borrower_id;
  }
  if (!this->device_ids.empty()) {
    j["deviceIds"] = this->device_ids;
  }
  return j;
}

User::User() {}

User::User(nlohmann::json json) {
  if (json.contains("id")) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("email")) {
    json.at("email").get_to(this->email);
  }
  if (json.contains("name")) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("organization")) {
    json.at("organization").get_to(this->organization);
  }
  if (json.contains("group")) {
    this->group = Group(json.at("group"));
  }
  if (json.contains("deviceIds")) {
    json.at("deviceIds").get_to(this->device_ids);
  }
  if (json.contains("leasesOwnedIds")) {
    json.at("leasesOwnedIds").get_to(this->leases_owned_ids);
  }
  if (json.contains("leasesBorrowedIds")) {
    json.at("leasesBorrowedIds").get_to(this->leases_borrowed_ids);
  }
}

nlohmann::json User::to_json() const {
  nlohmann::json j;
  if (!this->id.empty()) {
    j["id"] = this->id;
  }
  if (!this->email.empty()) {
    j["email"] = this->email;
  }
  if (!this->name.empty()) {
    j["name"] = this->name;
  }
  if (!this->organization.empty()) {
    j["organization"] = this->organization;
  }
  if (!this->group.id.empty()) {
    j["group"] = this->group.to_json();
  }
  if (!this->device_ids.empty()) {
    j["deviceIds"] = this->device_ids;
  }
  if (!this->leases_owned_ids.empty()) {
    j["leasesOwnedIds"] = this->leases_owned_ids;
  }
  if (!this->leases_borrowed_ids.empty()) {
    j["leasesBorrowedIds"] = this->leases_borrowed_ids;
  }
  return j;
}

}  // namespace viacast::central::entities
