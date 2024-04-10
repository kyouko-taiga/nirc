#ifndef NIRC_VALUE_H
#define NIRC_VALUE_H

#include "Local.hh"
#include "Runtime.hh"
#include "Symbol.hh"
#include "Type.hh"
#include "Utilities/Indirect.hh"

#include <algorithm>
#include <cstdlib>
#include <vector>

namespace nir {

  struct Value;

} // nir

namespace nir::value {

/// A trait describing the API of NIR values.
template<typename T>
struct ValueTrait {

  /// Returns the NIR type of `self`.
  static Type type(T const& self) { return self.type(); }

};

/// The constant null value.
struct Null final {

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type::null(); }

  /// Returns `true` if this instance is equal to `rhs`.
  inline bool operator==(Null const&) const { return true; }

};

/// The unit value.
struct Unit final {

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type::unit(); }

  /// Returns `true` if this instance is equal to `rhs`.
  inline bool operator==(Unit const&) const { return true; }

};

/// The "zero" value of the given NIR type.
struct Zero final {
private:

  /// The type of the value.
  Type _type;

public:

  /// Creates an instance having the given type.
  Zero(Type const& t) : _type(t) {}

  Zero() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return _type; }

  /// Returns `true` if this instance is equal to `rhs`.
  inline bool operator==(Zero const&) const { return true; }

};

/// A numerical value suitable to represent the size of a container.
struct Size final {

  /// The raw value of this instance.
  uint64_t raw_value;

  /// Creates an instance with the given raw value.
  Size(uint64_t v) : raw_value(v) {}

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type::size(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Size const& rhs) const = default;

};

/// 16-bit unsigned Unicode character.
using Boolean = bool;

/// 16-bit unsigned Unicode character.
using Char = uint16_t;

/// A 8-bit signed two’s complement integer.
using Byte = int8_t;

/// A 16-bit signed two’s complement integer.
using Short = int16_t;

/// A 32-bit signed two’s complement integer.
using Int = int32_t;

/// A 64-bit signed two’s complement integer.
using Long = int64_t;

/// A 32-bit IEEE 754 single-precision float.
using Float = float;

/// A 64-bit IEEE 754 double-precision float.
using Double = double;

/// A homogeneous collection of data members.
struct ArrayValue final {

  /// The type of the array's elements.
  Type element_type;

  /// The elements in the array.
  std::vector<indirect<Value>> elements;

  /// Creates an instance with `es`, which are instances of `t`.
  ArrayValue(Type const& t, std::vector<Value> const& es);

  ArrayValue() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return element_type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayValue const& rhs) const = default;

};

/// A heterogeneous collection of data members.
struct Struct final {

  /// The elements in the aggregate.
  std::vector<indirect<Value>> elements;

  /// Creates an instance with `es`.
  Struct(std::vector<Value> const& es);

  Struct() = delete;

  /// Returns the NIR type of this instance.
  Type type() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Struct const& rhs) const = default;

};

/// A collection of bytes.
struct ByteString final {

  /// The contents of the collection.
  std::vector<Byte> bytes;

  /// Creates an instance with the given bytes.
  ByteString(std::vector<Byte> const& bs) : bytes(bs) {}

  ByteString() = delete;

  /// Returns The number of bytes in the collection.
  inline std::size_t byte_count() const { return bytes.size() + 1; }

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type(type::ArrayValue(Type::i8(), byte_count())); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ByteString const& rhs) const = default;

};

/// A local SSA variable.
struct Local final {
private:

  /// The type of the variable.
  Type _type;

public:

  /// The identifier of the variable.
  nir::Local id;

  /// Creates an instance with the given properties.
  Local(nir::Local const& n, Type const& t) : _type(t), id(n) {}

  Local() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return _type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Local const& rhs) const = default;

};

/// A reference to a symbol.
struct Symbol final {
private:

  /// The type of the reference.
  Type _type;

public:

  /// The name of the referred symbol.
  nir::Symbol name;

  /// Creates an instance with the given properties.
  Symbol(nir::Symbol const& n, Type const& t) : _type(t), name(n) {}

  Symbol() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return _type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Symbol const& rhs) const = default;

};

/// A constant value.
struct Constant final {

  /// The value of the constant.
  indirect<Value> value;

  /// Creates an instance with the given properties.
  Constant(Value const& v) : value(indirect(v)) {}

  Constant() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type::pointer(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Constant const& rhs) const = default;

};

/// An instance of `java.lang.String`.
struct String final {

  /// The value of the character string.
  std::string value;

  /// Creates an instance with the given value.
  String(std::string const& v) : value(v) {}

  String() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type(type::Reference(runtime::String.name, true, false)); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(String const& rhs) const = default;

};

/// A virtual value.
struct Virtual final {

  /// A key identifying the value.
  std::size_t key;

  /// Creates an instance with the given key.
  Virtual(std::size_t k) : key(k) {}

  Virtual() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return Type::virtual_(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Virtual const& rhs) const = default;

};

/// A reference to `java.lang.Class[_]` of a given symbol `name`.
struct ClassOf final {

  /// The symbol whose class is referred to.
  symbol::Top name;

  /// Creates an instance with the given name.
  ClassOf(symbol::Top const& n) : name(n) {}

  ClassOf() = delete;

  /// Returns the NIR type of `self`.
  inline Type type() const { return runtime::Class; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ClassOf const& rhs) const = default;

};

} // nir::value

namespace nir {

/// A value in NIR.
struct Value final {
private:

  /// The internal representation of a value.
  using Representation = std::variant<
    value::Null,
    value::Unit,
    value::Zero,
    value::Boolean,
    value::Size,
    value::Char,
    value::Byte,
    value::Short,
    value::Int,
    value::Long,
    value::Float,
    value::Double,
    value::ArrayValue,
    value::Struct,
    value::ByteString,
    value::Local,
    value::Symbol,
    value::Constant,
    value::String,
    value::Virtual,
    value::ClassOf
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Value(T const& w) : wrapped(w) {}

  Value(Value const& other) : wrapped(other.wrapped) {}

  Value(Value&& other) : wrapped(std::move(other.wrapped)) {}

  Value() = delete;

  ~Value() noexcept {}

  Value& operator=(Value const& other) { return *this = Value(other); }

  Value& operator=(Value&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns `true` if `this` wraps an instance of `T`.
  template<typename T>
  inline bool is() const {
    return std::holds_alternative<T>(wrapped);
  }

  /// Returns the wrapped value if it is an instance of `T`.
  template<typename T>
  inline std::optional<T> as() const {
    return is<T>() ? std::optional(std::get<T>(wrapped)) : std::nullopt;
  }

  /// Returns the NIR type of this instance.
  Type type() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Value const& rhs) const = default;

};

} // nir

// --- Trait implementations --------------------------------------------------

namespace nir::value {

template<>
struct ValueTrait<Boolean> {
  inline static Type type(Boolean const&) { return Type::u1(); }
};

template<>
struct ValueTrait<Char> {
  inline static Type type(Char const&) { return Type::u16(); }
};

template<>
struct ValueTrait<Byte> {
  inline static Type type(Byte const&) { return Type::i8(); }
};

template<>
struct ValueTrait<Short> {
  inline static Type type(Short const&) { return Type::i16(); }
};

template<>
struct ValueTrait<Int> {
  inline static Type type(Int const&) { return Type::i32(); }
};

template<>
struct ValueTrait<Long> {
  inline static Type type(Long const&){ return Type::i64(); }
};

template<>
struct ValueTrait<Float> {
  inline static Type type(Float const&) { return Type::f32(); }
};

template<>
struct ValueTrait<Double> {
  inline static Type type(Double const&) { return Type::f64(); }
};

} // nir::value

#endif
