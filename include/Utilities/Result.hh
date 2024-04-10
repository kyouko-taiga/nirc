#ifndef NIRC_RESULT_H
#define NIRC_RESULT_H

#include <concepts>
#include <exception>
#include <optional>
#include <utility>
#include <variant>

namespace nir {

/// A value of type `T` or an `std::exception`.
template<typename T>
struct Result {
private:

  /// The internal representation of this instance.
  std::variant<T, std::exception> wrapped;

  /// Creates an instance wrapping `w`.
  explicit Result(std::variant<T, std::exception>&& w) : wrapped(std::move(w)) {}

public:

  /// Creates an instance wrapping the value `w`.
  inline static Result<T> success(T const& w) { return Result(w); }

  /// Creates an instance wrapping the value `w`.
  inline static Result<T> success(T&& w) { return Result(std::move(w)); }

  /// Creates an instance wrapping the error `e`.
  inline static Result<T> failure(std::exception&& e) { return Result(e); }

  /// Returns `true` if this instance wraps a value and `false` if it wraps an error.
  inline bool is_success() const { return wrapped.index() == 0; }

  /// Returns the value wrapped in this instance or throws the wrapped error.
  const T& get() const {
    if (wrapped.index() == 0) {
      return std::get<0>(wrapped);
    } else {
      throw std::get<1>(wrapped);
    }
  }

  /// If this instance wraps a value, returns a `Result` wrapping the result of `transform` applied
  /// on that value. Otherwise, returns a `Result` containing the wrapped error.
  template<typename U, typename F>
  requires std::invocable<F, T>
  Result<U> map(F&& transform) const {
    if (wrapped.index() == 0) {
      return Result<U>::success(transform(std::get<0>(wrapped)));
    } else {
      return Result<U>::failure(std::exception(std::get<1>(wrapped)));
    }
  }

  /// Returns `true` if this instance wraps a value equal to `rhs`.
  inline bool operator==(T const& rhs) const {
    return (wrapped.index() == 0) && (std::get<0>(wrapped) == rhs);
  }

  /// Returns `true` if this instance wraps an error or a value not equal to `rhs`.
  inline bool operator!=(T const& rhs) const {
    return (wrapped.index() != 0) || (std::get<0>(wrapped) != rhs);
  }

};

}; // nir

#endif
