#include "utils.hpp"

namespace viacast {
namespace central {
namespace utils {

void wait_forever() { std::promise<void>().get_future().wait(); }

}  // namespace utils
}  // namespace central
}  // namespace viacast
