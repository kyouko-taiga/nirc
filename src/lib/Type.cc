#include "Runtime.hh"
#include "Type.hh"
#include "Utilities/Assert.hh"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace nir::type {

std::ostream& operator<<(std::ostream& s, const Predefined& t) {
  switch (t) {
    case Predefined::null:
      return s << "null";
    case Predefined::unit:
      return s << "unit";
    case Predefined::pointer:
      return s << "ptr";
    case Predefined::size:
      return s << "size";
    case Predefined::vararg:
      return s << "...";
    case Predefined::nothing:
      return s << "nothing";
    case Predefined::virtual_:
      return s << "virtual";
    default:
      fatal_error("unreachable");
  }
}

Numeric Numeric::integer(uint16_t width, bool is_signed) {
  precondition(width < (1 << 14), "invalid integer width");
  return { static_cast<uint16_t>(width | ((is_signed ? 0b11 : 0b10) << 14)) };
}

Numeric Numeric::floating_point(uint16_t width) {
  precondition((width == 32) || (width == 64), "invalid floating-point width");
  return { width };
}

Type Numeric::normalized() const {
  return Type(*this);
}

std::ostream& operator<<(std::ostream& s, const Numeric& t) {
  if (t.is_integer()) {
    s << (t.is_signed() ? "i" : "u") << t.bit_width();
  } else {
    s << "f" << t.bit_width();
  }
  return s;
}

Type ArrayValue::normalized() const {
  return Type(ArrayValue(element->normalized(), size));
}

std::ostream& operator<<(std::ostream& s, const ArrayValue& t) {
  return s << *t.element << "[" << t.size <<  "]";
}

Type ArrayReference::normalized() const {
  return Type(ArrayReference(element->normalized(), true));
}

std::ostream& operator<<(std::ostream& s, const ArrayReference& t) {
  s << "Array[" << *t.element << "]";
  if (t.is_nullable) { s << "?"; }
  return s;
}

Struct::Struct(std::vector<Type> const& es) {
  std::transform(es.begin(), es.end(), elements.begin(), [](auto&& e) {
    return indirect(e);
  });
}

Type Struct::normalized() const {
  Struct r;
  r.elements.reserve(elements.size());
  std::transform(elements.begin(), elements.end(), r.elements.begin(), [](auto&& e) {
    return indirect(e->normalized());
  });
  return Type(std::move(r));
}

std::ostream& operator<<(std::ostream& s, const Struct& t) {
  s << "{";
  auto first = true;
  for (auto& e : t.elements) {
    if (!first) { s << ", "; } else { first = false; }
    s << *e;
  }
  return s << "}";
}

Type Reference::normalized() const {
  return Type(Reference(name, false, true));
}

std::ostream& operator<<(std::ostream& s, const Reference& t) {
  s << t.name;
  if (t.is_exact) { s << "!"; }
  if (t.is_nullable) { s << "?"; }
  return s;
}

Type Var::normalized() const {
  return Type(Var(type->normalized()));
}

std::ostream& operator<<(std::ostream& s, const Var& t) {
  return s << "var[" << *t.type << "]";
}

Function::Function(std::vector<Type> const& ps, Type const& rv) : return_value(rv) {
  parameters.reserve(ps.size());
  std::transform(ps.begin(), ps.end(), parameters.begin(), [](auto&& p) {
    return indirect(p);
  });
}

Type Function::normalized() const {
  Function r{{}, return_value->normalized()};
  r.parameters.reserve(parameters.size());
  std::transform(parameters.begin(), parameters.end(), r.parameters.begin(), [](auto&& p) {
    return indirect(p->normalized());
  });
  return Type(std::move(r));
}

std::ostream& operator<<(std::ostream& s, const Function& t) {
  s << "(";
  auto first = true;
  for (auto& p : t.parameters) {
    if (!first) { s << ", "; } else { first = false; }
    s << *p;
  }
  return s << ") => " << *t.return_value;
}

} // nir::type

namespace nir {

/// Returns the name of class representing arrays of `t`.
symbol::Top to_array_class(Type const& self) {
  if (self == Type::u1()) {
    return runtime::BooleanArray.name;
  } else if (self == Type::u16()) {
    return runtime::CharArray.name;
  } else if (self == Type::i8()) {
    return runtime::ByteArray.name;
  } else if (self == Type::i16()) {
    return runtime::ShortArray.name;
  } else if (self == Type::i32()) {
    return runtime::IntArray.name;
  } else if (self == Type::i64()) {
    return runtime::LongArray.name;
  } else if (self == Type::f32()) {
    return runtime::FloatArray.name;
  } else if (self == Type::f64()) {
    return runtime::DoubleArray.name;
  } else {
    return runtime::ObjectArray.name;
  }
}

Type Type::element_at(uint32_t i) const {
  if (is<type::ArrayValue>()) {
    auto& w = std::get<type::ArrayValue>(wrapped);
    if (i >= w.size) { throw std::out_of_range("index is out of range"); }
    return *(w.element);
  }

  else if (is<type::Struct>()) {
    return *(std::get<type::Struct>(wrapped).elements.at(i));
  }

  else {
    std::stringstream diagnostic;
    diagnostic << "type '" << *this << "' does not implement 'element_at'";
    throw std::domain_error(diagnostic.str());
  }
}

/// Throws an exception caused by a failure to resolve the class corresponding to a type.
[[noreturn]] void no_class_name(Type const& self) {
  std::stringstream diagnostic;
  diagnostic << "type '" << self << "' has no corresponding class";
  throw std::domain_error(diagnostic.str());
}

symbol::Top Type::class_name() const {
  if (is<type::Predefined>()) {
    switch (std::get<type::Predefined>(wrapped)) {
      case type::Predefined::null:
        return runtime::BoxedNull.name;
      case type::Predefined::unit:
        return runtime::BoxedUnit.name;
      default: {
        no_class_name(*this);
      }
    }
  }

  else if (is<type::ArrayReference>()) {
    return to_array_class(*(std::get<type::ArrayReference>(wrapped).element));
  }

  else if (is<type::Reference>()) {
    return std::get<type::Reference>(wrapped).name;
  }

  else { no_class_name(*this); }
}

std::optional<Type> Type::unboxed() const {
  if (!is<type::Reference>()) { return std::nullopt; }

  auto n = as<type::Reference>().value().name.id;
  if (n == "scala.scalanative.unsafe.CArray") {
    return Type::pointer();
  } else if (n == "scala.scalanative.unsafe.CVarArgList") {
    return Type::pointer();
  } else if (n == "scala.scalanative.unsafe.Ptr") {
    return Type::pointer();
  } else if (n == "scala.scalanative.unsafe.Size") {
    return Type::size();
  } else if (n == "java.lang.Boolean") {
    return Type::u1();
  } else if (n == "java.lang.Character") {
    return Type::u16();
  } else if (n == "java.lang.Byte") {
    return Type::i8();
  } else if (n == "java.lang.Short") {
    return Type::i16();
  } else if (n == "java.lang.Integer") {
    return Type::i32();
  } else if (n == "java.lang.Long") {
    return Type::i64();
  } else if (n == "java.lang.Float") {
    return Type::f32();
  } else if (n == "java.lang.Double") {
    return Type::f64();
  } else if (n.starts_with("scala.scalanative.unsafe.CFuncPtr")) {
    auto s = n.substr(33);
    for (std::size_t i = 0; i < 22; ++i) {
      if (s == std::to_string(i)) { return Type::pointer(); }
    }
    return std::nullopt;
  } else {
    return std::nullopt;
  }
}

Type Type::normalized() const {
  return std::visit([](auto&& self) {
    using Self = std::decay_t<decltype(self)>;
    return type::TypeTrait<Self>::normalized(self);
  }, wrapped);
}

bool Type::is_box_of(Type const& t) const {
  return normalized().unboxed() == t;
}

bool Type::is_pointer_box() const {
  return is_box_of(Type::pointer());
}

bool Type::is_reference() const {
  if (is<type::Predefined>()) {
    switch (std::get<type::Predefined>(wrapped)) {
      case type::Predefined::null:
      case type::Predefined::unit:
        return true;
      default:
        return false;
    }
  } else {
    return is<type::Reference>() || is<type::ArrayReference>();
  }
}

bool Type::has_known_size() const {
  if (is<type::Predefined>()) {
    auto& w = std::get<type::Predefined>(wrapped);
    return (w == type::Predefined::null) || (w == type::Predefined::pointer);
  } else if (is<type::ArrayValue>()) {
    auto& w = std::get<type::ArrayValue>(wrapped);
    return w.element->has_known_size();
  } else if (is<type::Struct>()) {
    auto& w = std::get<type::Struct>(wrapped);
    return std::all_of(
      w.elements.begin(), w.elements.end(), [](auto& e) { return e->has_known_size(); });
  } else {
    return !is_reference();
  }
}

std::ostream& operator<<(std::ostream& s, const Type& wrapper) {
  std::visit([&](auto&& self) { s << self; }, wrapper.wrapped);
  return s;
}

} // nir
