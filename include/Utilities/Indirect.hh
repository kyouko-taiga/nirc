#ifndef NIRC_INDIRECT_H
#define NIRC_INDIRECT_H

#include <memory>

namespace nir {

/// A box wrapping a copy-constructible value allocated on the heap.
///
/// Use `indirect` to store a value in a recursive data type while preserving value semantics.
template<typename T>
struct indirect {
private:

  /// The wrapped value.
  std::unique_ptr<T> wrapped;

public:

  /// Creates a copy of `other`.
  indirect(T const& other) : wrapped(std::make_unique<T>(other)) {}

  /// Creates a copy of `other`.
  indirect(T&& other) : wrapped(std::make_unique<T>(std::move(other))) {}

  /// Creates a copy of `other`.
  indirect(indirect const& other) : wrapped(std::make_unique<T>(*other.wrapped)) {}

  /// Creates an instance by consuming `other`.
  indirect(indirect&& other) : wrapped(std::move(other.wrapped)) {}

  /// Assigns `this` to a copy of `other`.
  indirect& operator=(indirect const& other) {
    return *this = { other };
  }

  /// Assigns `this` to `other`, consuming it.
  indirect& operator=(indirect&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Accesses the wrapped value.
  T& operator*() { return *wrapped; }

  /// Accesses the wrapped value.
  T const& operator*() const { return *wrapped; }

  /// Accesses the wrapped value.
  T* operator->() { return wrapped.get(); }

  /// Accesses the wrapped value.
  const T* operator->() const { return wrapped.get(); }

  /// Returns `true` if this instance is equal to `rhs`.
  inline bool operator==(indirect<T> const& rhs) const { return *(*this) == *rhs; };

};

}

#endif