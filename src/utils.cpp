#include "utils.hpp"

namespace viacast::central::utils {

void wait_forever() { std::promise<void>().get_future().wait(); }

}  // namespace viacast::central::utils
