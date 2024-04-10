#include "Utilities/Assert.hh"

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string_view>

namespace nir {

void report_diagnostic(
  const char* title, const std::string_view message,
  std::ostream& output,
  const std::source_location location
) {
  output << location.file_name() << ":" << location.line() << ": " << title;
  if (!message.empty()) {
    output << ": " << message << "\n";
  } else {
    output << "\n";
  }
}

[[noreturn]] void fatal_error(
  const std::string_view message,
  std::ostream& output,
  const std::source_location location
) {
  report_diagnostic("fatal error", message, output, location);
  std::abort();
}

void precondition(
  bool condition,
  const std::string_view message,
  std::ostream& output,
  const std::source_location location
) {
  if (condition) { return; }
  report_diagnostic("precondition failure", message, output, location);
  std::abort();
}

} // nir
