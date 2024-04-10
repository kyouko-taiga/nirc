#include "Operation.hh"

namespace nir {

Type Operation::result_type() const {
  return std::visit([](auto&& self) {
    using Self = std::decay_t<decltype(self)>;
    return OperationTrait<Self>::result_type(self);
  }, wrapped);
}

} // std
