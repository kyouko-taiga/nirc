#ifndef NIRC_ASSERT_H
#define NIRC_ASSERT_H

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string_view>

namespace nir {

/// Unconditionally stops execution.
[[noreturn]] void fatal_error(
  const std::string_view message = "",
  std::ostream& output = std::cerr,
  const std::source_location location = std::source_location::current()
);

/// Checks a necessary condition for making forward progress.
void precondition(
  bool condition, const std::string_view message = "",
  std::ostream& output = std::cerr,
  const std::source_location location = std::source_location::current()
);

} // nir

#endif
