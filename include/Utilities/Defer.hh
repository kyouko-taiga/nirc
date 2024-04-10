#ifndef NIRC_DEFER_H
#define NIRC_DEFER_H

#include <utility>

namespace nir {

/// An object that runs a function at the end of its lifetime.
template<typename A>
struct defer {
private:

  /// The action to run when this object goes out of scope.
  A action;

public:

  /// Creates an object that executes `action` at the end of its lifetime.
  defer(A const& action) : action(action) {}

  /// Creates an object that executes `action` at the end of its lifetime.
  defer(A&& action) : action(std::move(action)) {}

  defer(defer const&) = delete;
  defer(defer&&) = delete;

  defer& operator=(defer const&) = delete;
  defer& operator=(defer&&) = delete;

  ~defer() { action(); }

};

}

#endif