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
  try {
    return names.at(role);
  } catch (std::out_of_range const &) {
    return "";
  }
}

std::string to_string(ServiceType type) {
  std::map<ServiceType, std::string> names = {{ServiceType::FEED, "FEED"},
                                              {ServiceType::OUTPUT, "OUTPUT"}};
  try {
    return names.at(type);
  } catch (std::out_of_range const &) {
    return "";
  }
}

std::string to_string(ServiceStatusType type) {
  std::map<ServiceStatusType, std::string> names = {
      {ServiceStatusType::OFF, "OFF"},
      {ServiceStatusType::READY, "READY"},
      {ServiceStatusType::LOAD, "LOAD"},
      {ServiceStatusType::LIVE, "LIVE"}};
  try {
    return names.at(type);
  } catch (std::out_of_range const &) {
    return "";
  }
}

std::string to_string(DeviceStatusType type) {
  std::map<DeviceStatusType, std::string> names = {
      {DeviceStatusType::OFF, "OFF"},
      {DeviceStatusType::ON, "ON"},
      {DeviceStatusType::REBOOTING, "REBOOTING"}};
  try {
    return names.at(type);
  } catch (std::out_of_range const &) {
    return "";
  }
}

std::string to_string(LeaseType type) {
  std::map<LeaseType, std::string> names = {
      {LeaseType::EXCLUSIVE, "EXCLUSIVE"},
      {LeaseType::NON_EXCLUSIVE, "NON_EXCLUSIVE"}};
  try {
    return names.at(type);
  } catch (std::out_of_range const &) {
    return "";
  }
}

std::string to_string(ToggleRunningAction action) {
  std::map<ToggleRunningAction, std::string> names = {
      {ToggleRunningAction::START, "start"},
      {ToggleRunningAction::STOP, "stop"},
      {ToggleRunningAction::RESTART, "restart"}};
  try {
    return names.at(action);
  } catch (std::out_of_range const &) {
    return "";
  }
}

}  // namespace viacast::central::types

namespace viacast::central::entities {

Auth::Auth() {}

Auth::Auth(nlohmann::json json) {
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("key") && !json.at("key").is_null()) {
    json.at("key").get_to(this->key);
  }
  if (json.contains("role") && !json.at("role").is_null()) {
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
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("displayName") && !json.at("displayName").is_null()) {
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
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("name") && !json.at("name").is_null()) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("displayName") && !json.at("displayName").is_null()) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("type") && !json.at("type").is_null()) {
    viacast::central::types::from_string(json.at("type").get<std::string>(),
                                         &this->type);
  }
  if (json.contains("configLayout") && !json.at("configLayout").is_null()) {
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
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("name") && !json.at("name").is_null()) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("displayName") && !json.at("displayName").is_null()) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("config") && !json.at("config").is_null()) {
    json.at("config").get_to(this->config);
  }
  if (json.contains("operationMode") && !json.at("operationMode").is_null()) {
    this->operation_mode = ServiceOperationMode(json.at("operationMode"));
  }
  if (json.contains("operationModeId") &&
      !json.at("operationModeId").is_null()) {
    json.at("operationModeId").get_to(this->operation_mode_id);
  }
  if (json.contains("linkedServiceIds") &&
      !json.at("linkedServiceIds").is_null()) {
    json.at("linkedServiceIds").get_to(this->linked_service_ids);
  }
  if (json.contains("deviceId") && !json.at("deviceId").is_null()) {
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
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("status") && !json.at("status").is_null()) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  }
  if (json.contains("info") && !json.at("info").is_null()) {
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

ServiceWithStatus::ServiceWithStatus() {
  this->status = types::ServiceStatusType::null;
}

ServiceWithStatus::ServiceWithStatus(nlohmann::json json) {
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("name") && !json.at("name").is_null()) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("displayName") && !json.at("displayName").is_null()) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("config") && !json.at("config").is_null()) {
    json.at("config").get_to(this->config);
  }
  if (json.contains("operationMode") && !json.at("operationMode").is_null()) {
    this->operation_mode = ServiceOperationMode(json.at("operationMode"));
  }
  if (json.contains("operationModeId") &&
      !json.at("operationModeId").is_null()) {
    json.at("operationModeId").get_to(this->operation_mode_id);
  }
  if (json.contains("linkedServiceIds") &&
      !json.at("linkedServiceIds").is_null()) {
    json.at("linkedServiceIds").get_to(this->linked_service_ids);
  }
  if (json.contains("deviceId") && !json.at("deviceId").is_null()) {
    json.at("deviceId").get_to(this->device_id);
  }
  if (json.contains("status") && !json.at("status").is_null()) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  } else {
    this->status = types::ServiceStatusType::null;
  }
  if (json.contains("info") && !json.at("info").is_null()) {
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

void ServiceWithStatus::merge(ServiceWithStatus service) {
  if (!service.id.empty()) {
    this->id = id;
  }
  if (!service.name.empty()) {
    this->name = service.name;
  }
  if (!service.display_name.empty()) {
    this->display_name = service.display_name;
  }
  if (!service.config.empty()) {
    this->config = service.config;
  }
  if (!service.operation_mode.id.empty()) {
    this->operation_mode = service.operation_mode;
  }
  if (!service.operation_mode_id.empty()) {
    this->operation_mode_id = service.operation_mode_id;
  }
  if (!service.linked_service_ids.empty()) {
    if (service.linked_service_ids.front().empty()) {
      this->linked_service_ids = {};
    } else {
      this->linked_service_ids = service.linked_service_ids;
    }
  }
  if (!service.device_id.empty()) {
    this->device_id = service.device_id;
  }
  if (service.status != viacast::central::types::ServiceStatusType::null) {
    this->status = service.status;
  }
  if (!service.info.empty()) {
    this->info = service.info;
  }
}

void ServiceWithStatus::ServiceWithStatus::operator=(Service const &s) {
  this->id = s.id;
  this->name = s.name;
  this->display_name = s.display_name;
  this->config = s.config;
  this->operation_mode = s.operation_mode;
  this->operation_mode_id = s.operation_mode_id;
  this->linked_service_ids = s.linked_service_ids;
  this->device_id = s.device_id;
}

Device::Device() {}

Device::Device(nlohmann::json json) {
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("displayName") && !json.at("displayName").is_null()) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("serial") && !json.at("serial").is_null()) {
    json.at("serial").get_to(this->serial);
  }
  if (json.contains("userId") && !json.at("userId").is_null()) {
    json.at("userId").get_to(this->user_id);
  }
  if (json.contains("model") && !json.at("model").is_null()) {
    json.at("model").get_to(this->model);
  }
  if (json.contains("group") && !json.at("group").is_null()) {
    this->group = Group(json.at("group"));
  }
  if (json.contains("serviceIds") && !json.at("serviceIds").is_null()) {
    json.at("serviceIds").get_to(this->service_ids);
  }
  if (json.contains("leaseIds") && !json.at("leaseIds").is_null()) {
    json.at("leaseIds").get_to(this->lease_ids);
  }
  if (json.contains("serviceOperationModes") &&
      !json.at("serviceOperationModes").is_null()) {
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
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("status") && !json.at("status").is_null()) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  }
  if (json.contains("info") && !json.at("info").is_null()) {
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

DeviceWithStatus::DeviceWithStatus() {
  this->status = types::DeviceStatusType::null;
}

DeviceWithStatus::DeviceWithStatus(nlohmann::json json) {
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("displayName") && !json.at("displayName").is_null()) {
    json.at("displayName").get_to(this->display_name);
  }
  if (json.contains("serial") && !json.at("serial").is_null()) {
    json.at("serial").get_to(this->serial);
  }
  if (json.contains("userId") && !json.at("userId").is_null()) {
    json.at("userId").get_to(this->user_id);
  }
  if (json.contains("model") && !json.at("model").is_null()) {
    json.at("model").get_to(this->model);
  }
  if (json.contains("group") && !json.at("group").is_null()) {
    this->group = Group(json.at("group"));
  }
  if (json.contains("serviceIds") && !json.at("serviceIds").is_null()) {
    json.at("serviceIds").get_to(this->service_ids);
  }
  if (json.contains("leaseIds") && !json.at("leaseIds").is_null()) {
    json.at("leaseIds").get_to(this->lease_ids);
  }
  if (json.contains("serviceOperationModes") &&
      !json.at("serviceOperationModes").is_null()) {
    for (auto op : json.at("serviceOperationModes")) {
      this->service_operation_modes.push_back(ServiceOperationMode(op));
    }
  }
  if (json.contains("status") && !json.at("status").is_null()) {
    viacast::central::types::from_string(json.at("status").get<std::string>(),
                                         &this->status);
  } else {
    this->status = types::DeviceStatusType::null;
  }
  if (json.contains("info") && !json.at("info").is_null()) {
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

void DeviceWithStatus::merge(DeviceWithStatus device) {
  if (!device.id.empty()) {
    this->id = id;
  }
  if (!device.display_name.empty()) {
    this->display_name = device.display_name;
  }
  if (!device.serial.empty()) {
    this->serial = device.serial;
  }
  if (!device.user_id.empty()) {
    this->user_id = device.user_id;
  }
  if (!device.model.empty()) {
    this->model = device.model;
  }
  if (!device.group.id.empty()) {
    this->group = device.group;
  }
  if (!device.service_ids.empty()) {
    if (device.service_ids.front().empty()) {
      this->service_ids = {};
    } else {
      this->service_ids = device.service_ids;
    }
  }
  if (!device.lease_ids.empty()) {
    if (device.lease_ids.front().empty()) {
      this->lease_ids = {};
    } else {
      this->lease_ids = device.lease_ids;
    }
  }
  if (!device.service_operation_modes.empty()) {
    if (device.service_operation_modes.front().name.empty()) {
      this->service_operation_modes = {};
    } else {
      this->service_operation_modes = device.service_operation_modes;
    }
  }
  if (device.status != viacast::central::types::DeviceStatusType::null) {
    this->status = device.status;
  }
  if (!device.info.empty()) {
    this->info = device.info;
  }
}

void DeviceWithStatus::DeviceWithStatus::operator=(Device const &d) {
  this->id = d.id;
  this->display_name = d.display_name;
  this->serial = d.serial;
  this->user_id = d.user_id;
  this->model = d.model;
  this->group = d.group;
  this->service_ids = d.service_ids;
  this->lease_ids = d.lease_ids;
  this->service_operation_modes = d.service_operation_modes;
}

Lease::Lease() {}

Lease::Lease(nlohmann::json json) {
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("annotations") && !json.at("annotations").is_null()) {
    json.at("annotations").get_to(this->annotations);
  }
  if (json.contains("type") && !json.at("type").is_null()) {
    viacast::central::types::from_string(json.at("type").get<std::string>(),
                                         &this->type);
  }
  if (json.contains("start") && !json.at("start").is_null()) {
    json.at("start").get_to(this->start);
  }
  if (json.contains("expiration") && !json.at("expiration").is_null()) {
    json.at("expiration").get_to(this->expiration);
  }
  if (json.contains("canceled") && !json.at("canceled").is_null()) {
    json.at("canceled").get_to(this->canceled);
  }
  if (json.contains("ownerId") && !json.at("ownerId").is_null()) {
    json.at("ownerId").get_to(this->owner_id);
  }
  if (json.contains("borrowerId") && !json.at("borrowerId").is_null()) {
    json.at("borrowerId").get_to(this->borrower_id);
  }
  if (json.contains("deviceIds") && !json.at("deviceIds").is_null()) {
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
  if (json.contains("id") && !json.at("id").is_null()) {
    json.at("id").get_to(this->id);
  }
  if (json.contains("email") && !json.at("email").is_null()) {
    json.at("email").get_to(this->email);
  }
  if (json.contains("name") && !json.at("name").is_null()) {
    json.at("name").get_to(this->name);
  }
  if (json.contains("organization") && !json.at("organization").is_null()) {
    json.at("organization").get_to(this->organization);
  }
  if (json.contains("group") && !json.at("group").is_null()) {
    this->group = Group(json.at("group"));
  }
  if (json.contains("deviceIds") && !json.at("deviceIds").is_null()) {
    json.at("deviceIds").get_to(this->device_ids);
  }
  if (json.contains("leasesOwnedIds") && !json.at("leasesOwnedIds").is_null()) {
    json.at("leasesOwnedIds").get_to(this->leases_owned_ids);
  }
  if (json.contains("leasesBorrowedIds") &&
      !json.at("leasesBorrowedIds").is_null()) {
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
