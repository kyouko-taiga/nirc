#ifndef NIRC_SCOPE_H
#define NIRC_SCOPE_H

#include "SourcePosition.hh"

#include <cstdlib>

namespace nir {

/// The identifier of a lexical scope.
struct ScopeIdentifier final {

  /// The raw value of the identifier.
  uint64_t raw_value;

  /// Returns `true` if this instance is the top-level scope.
  inline bool is_top_level() const { return raw_value == 0; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ScopeIdentifier const& rhs) const = default;

  /// The top-level scope.
  inline static ScopeIdentifier top_level() { return { 0 }; }

};

/// A lexical scope in program sources.
struct LexicalScope final {

  /// The identifier of the scope.
  ScopeIdentifier id;

  /// The innermost enclosing scope.
  ScopeIdentifier parent;

  /// The source position corresponding to the scope.
  SourcePosition position;

};

}; // nir

#endif
