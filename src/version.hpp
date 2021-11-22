#pragma once

#include <string>

#define CENTRAL_API_VERSION_DATE "20211122"
#define CENTRAL_API_VERSION_MAJOR "0"
#define CENTRAL_API_VERSION_MINOR "0"
#define CENTRAL_API_VERSION_PATCH "1"
#define CENTRAL_API_COMMIT_HASH "de3b2c0c09cacc3f87faf30dfbfe67367e9ff6fa"

namespace viacast::central {

std::string version();
std::string commit();

}  // namespace viacast::central
