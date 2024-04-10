#include "Signature.hh"

namespace nir {

std::ostream& operator<<(std::ostream& s, const Signature& self) {
  return s << self.mangled_name;
}

}

namespace std {

size_t hash<nir::Signature>::operator()(nir::Signature const& self) const {
  return hash<string>{}(self.mangled_name);
}

} // std
