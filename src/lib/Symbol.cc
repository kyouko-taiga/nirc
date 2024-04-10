#include "Symbol.hh"
#include "Utilities/Assert.hh"

namespace nir::symbol {

std::ostream& operator<<(std::ostream& s, const Top& self) {
  return s << self.id;
}

std::ostream& operator<<(std::ostream& s, const Member& self) {
  // TODO: Should be the mangled name.
  return s << self.top << "." << self.signature;
}

} // nir::symbol

namespace nir {

std::ostream& operator<<(std::ostream& s, const Symbol& self) {
  switch (self.wrapped.index()) {
    case 0: return s << "null";
    case 1: return s << std::get<1>(self.wrapped);
    case 2: return s << std::get<2>(self.wrapped);
    default:
      nir::fatal_error("unreachable");
  }
}

} // nir

namespace std {

size_t hash<nir::symbol::Top>::operator()(nir::symbol::Top const& self) const {
  return std::hash<string>{}(self.id);
}

size_t hash<nir::symbol::Member>::operator()(nir::symbol::Member const& self) const {
  return hash<nir::symbol::Top>{}(self.top) ^ hash<nir::Signature>{}(self.signature);
}

size_t hash<nir::Symbol>::operator()(nir::Symbol const& self) const {
  switch (self.wrapped.index()) {
    case 0: return hash<int>{}(0);
    case 1: return hash<nir::symbol::Top>{}(std::get<1>(self.wrapped));
    case 2: return hash<nir::symbol::Member>{}(std::get<2>(self.wrapped));
    default:
      nir::fatal_error("unreachable");
  }
}

} // std
