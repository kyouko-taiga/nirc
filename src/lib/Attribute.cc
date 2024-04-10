#include "Attribute.hh"
#include "Utilities/Assert.hh"

namespace nir::attribute {

std::ostream& operator<<(std::ostream& s, Kind self) {
  switch (self) {
    case Kind::may_inline:
      return s << "mayinline";
    case Kind::inline_hint:
      return s << "inlinehint";
    case Kind::no_inline:
      return s << "noinline";
    case Kind::always_inline:
      return s << "alwaysinline";
    case Kind::may_specialize:
      return s << "mayspecialize";
    case Kind::no_specialize:
      return s << "nospecialize";
    case Kind::un_opt:
      return s << "unopt";
    case Kind::no_opt:
      return s << "noopt";
    case Kind::did_opt:
      return s << "didopt";
    case Kind::bail_opt:
      return s << "bailopt";
    case Kind::dyn:
      return s << "dyn";
    case Kind::stub:
      return s << "stub";
    case Kind::extern_:
      return s << "extern";
    case Kind::link:
      return s << "link";
    case Kind::define:
      return s << "define";
    case Kind::abstract:
      return s << "abstract";
    case Kind::volatile_:
      return s << "volatile";
    case Kind::final:
      return s << "final";
    case Kind::safe_publish:
      return s << "safe-publish";
    case Kind::link_time_resolved:
      return s << "linktime";
    case Kind::uses_intrinsic:
      return s << "uses-intrinsics";
    case Kind::alignment:
      return s << "align";
    default:
      fatal_error("unreachable");
  }
}

std::ostream& operator<<(std::ostream& s, BailOpt const& self) {
  return s << Kind::bail_opt << "(" << self.message << ")";
}

std::ostream& operator<<(std::ostream& s, Extern const& self) {
  s << Kind::extern_;
  if (self.is_blocking) { s << " blocking"; }
  return s;
}

std::ostream& operator<<(std::ostream& s, Link const& self) {
  return s << Kind::link << "(" << self.name << ")";
}

std::ostream& operator<<(std::ostream& s, Define const& self) {
  return s << Kind::define << "(" << self.name << ")";
}

std::ostream& operator<<(std::ostream& s, Alignment const& self) {
  s << Kind::alignment << "(" << self.size;
  if (self.group != std::nullopt) { s << ", " << self.group.value(); }
  s << ")";
  return s;
}

} // nir::namespapce

namespace nir {

attribute::Kind Attribute::kind() const {
  return std::visit([](auto&& self) {
    using Self = std::decay_t<decltype(self)>;
    return attribute::AttributeTrait<Self>::kind(self);
  }, wrapped);
}

std::ostream& operator<<(std::ostream& s, const Attribute& wrapper) {
  std::visit([&](auto&& self) { s << self; }, wrapper.wrapped);
  return s;
}

} // nir
