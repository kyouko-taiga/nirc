#ifndef NIRC_SIGNATURE_H
#define NIRC_SIGNATURE_H

#include <string>

namespace nir {

/// The signature of a symbol.
struct Signature {

  friend std::ostream& operator<<(std::ostream&, const Signature&);

  /// The mangled name of the signature.
  std::string mangled_name;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Signature const& rhs) const = default;

};

} // nir

namespace std {

template<>
struct hash<nir::Signature> {

  /// Returns a hash of `self`'s salient properties.
  size_t operator()(nir::Signature const& self) const;

};

} // std

#endif
