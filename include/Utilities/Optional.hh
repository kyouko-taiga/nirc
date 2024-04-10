#ifndef NIRC_OPTIONAL_H
#define NIRC_OPTIONAL_H

#include <optional>

namespace nir {

/// If `o` contains a value `v`, returns `f(v)`. Otherwise, returns `std::nullopt`,
template<typename T, typename F>
constexpr auto map(T&& o, F&& f) -> std::optional<decltype(f(*std::forward<T>(o)))> {
  if (o.has_value()) {
    return { f(*std::forward<T>(o)) };
  } else {
    return { std::nullopt };
  }
}

}

#endif
