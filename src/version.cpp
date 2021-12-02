#include "version.hpp"

namespace viacast {
namespace central {

std::string version() {
  return "v" + std::string(CENTRAL_API_VERSION_DATE) + "_" +
         std::string(CENTRAL_API_VERSION_MAJOR) + "." +
         std::string(CENTRAL_API_VERSION_MINOR) + "." +
         std::string(CENTRAL_API_VERSION_PATCH);
}

std::string commit() { return std::string(CENTRAL_API_COMMIT_HASH); }

}  // namespace central
}  // namespace viacast
