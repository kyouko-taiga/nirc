#include "Value.hh"

#include <algorithm>

namespace nir::value {

ArrayValue::ArrayValue(Type const& t, std::vector<Value> const& es) : element_type(t) {
  elements.reserve(es.size());
  std::transform(es.begin(), es.end(), elements.begin(), [](auto const& e) {
    return indirect<Value>(e);
  });
}

Struct::Struct(std::vector<Value> const& es) {
  elements.reserve(es.size());
  std::transform(es.begin(), es.end(), elements.begin(), [](auto const& e) {
    return indirect<Value>(e);
  });
}

Type Struct::type() const {
  std::vector<indirect<Type>> es;
  es.reserve(elements.size());
  std::transform(elements.begin(), elements.end(), es.begin(), [](auto& e) {
    return indirect<Type>(e->type());
  });
  return Type(type::Struct(std::move(es)));
}

} // nir::value

namespace nir {

Type Value::type() const {
  return std::visit([](auto&& self) {
    using T = std::decay_t<decltype(self)>;
    return value::ValueTrait<T>::type(self);
  }, wrapped);
}

} // nir
