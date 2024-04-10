#ifndef NIRC_NEXT_H
#define NIRC_NEXT_H

#include "Local.hh"
#include "Value.hh"
#include "Utilities/Indirect.hh"

#include <vector>

namespace nir {

struct Next;

} // nir

namespace nir::next {

/// TODO
struct None final {

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(None const& rhs) const = default;

};

/// TODO
struct Unwind final {

  value::Local exception;

  indirect<Next> next;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Unwind const& rhs) const = default;

};

/// TODO
struct Case final {

  Value value;

  indirect<Next> next;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Case const& rhs) const = default;

};

/// TODO
struct Label final {

  Local id;

  std::vector<Value> arguments;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Label const& rhs) const = default;

};

} // nir::next

namespace nir {

/// TODO.
struct Next final {
private:

  /// The internal representation of a continuation.
  using Representation = std::variant<
    next::None,
    next::Unwind,
    next::Case,
    next::Label
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Next(T const& w) : wrapped(w) {}

  Next(Next const& other) : wrapped(other.wrapped) {}

  Next(Next&& other) : wrapped(std::move(other.wrapped)) {}

  Next() = delete;

  ~Next() noexcept {}

  Next& operator=(Next const& other) { return *this = Next(other); };

  Next& operator=(Next&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns `true` if `this` wraps an instance of `T`.
  template<typename T>
  inline bool is() const {
    return std::holds_alternative<T>(wrapped);
  }

  /// Projects `this` as an instance of `T` or returns `std::nullopt` if it's diffent kind of type.
  template<typename T>
  inline std::optional<T> as() const {
    return is<T>() ? std::optional(std::get<T>(wrapped)) : std::nullopt;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Next const& rhs) const = default;

  /// Returns the none symbol.
  inline static Next none() {
    return { next::None{} };
  }

};

} // nir


#endif
