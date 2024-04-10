#ifndef NIRC_TYPE_H
#define NIRC_TYPE_H

#include "Symbol.hh"
#include "Tags.hh"
#include "Utilities/Indirect.hh"

#include <algorithm>
#include <cstdlib>
#include <optional>
#include <ranges>
#include <variant>
#include <vector>

namespace nir {

  struct Type;

} // nir

namespace nir::type {

/// A predefined type symbol.
enum struct Predefined {

  null,
  unit,
  pointer,
  size,
  vararg,
  nothing,
  virtual_,

};

/// The type of a number.
struct Numeric {
private:

  friend std::ostream& operator<<(std::ostream&, const Numeric&);

  /// The raw representation of this instance.
  ///
  /// The most significant bit is set if the type represents integers, in which case the next bit
  /// encodes signedness. In any case the 30 least significant bits encode a bit width.
  uint16_t raw_value;

  /// Creates an instance with its raw value.
  Numeric(uint16_t v) : raw_value(v) {}

public:

  Numeric() = delete;

  /// Creates an instance representing the type of integers having `width` bits and a signed
  /// representation if and only if `is_signed` is `true`.
  ///
  /// - Precondition: `width` is smaller than 2^14.
  static Numeric integer(uint16_t width, bool is_signed);

  /// Creates an instance representing the type of floating-point numbers habing `width` bits.
  ///
  /// - Precondition: `width` is equal to either `32` or `64`.
  static Numeric floating_point(uint16_t width);

  /// Returns `true` if `this` denotes an integer.
  inline bool is_integer() const { return (raw_value & (1 << 15)); }

  /// Returns `true` if `this` denotes a signed integer.
  inline bool is_signed() const { return (raw_value & (1 << 14)); }

  /// Returns `true` if `this` denotes a floating-point.
  inline bool is_floating_point() const { return !is_integer(); }

  /// Returns the number of bits used in instances of the denoted type.
  inline uint32_t bit_width() const { return (raw_value & ~(0b11 << 14)); }

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Numeric const& rhs) const = default;

};

/// The type of an array value.
struct ArrayValue {

  friend std::ostream& operator<<(std::ostream&, const ArrayValue&);

  /// The type of the array's elements.
  indirect<Type> element;

  /// The size of the array.
  std::size_t size;

  /// Creates an instance representing an array of `size` instances of `element`.
  ArrayValue(Type const& element, std::size_t size) : element(element), size(size) {}

  ArrayValue() = delete;

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayValue const& rhs) const = default;

};

/// The type of an array reference.
struct ArrayReference {

  friend std::ostream& operator<<(std::ostream&, const ArrayValue&);

  /// The type of the array's elements.
  indirect<Type> element;

  /// `true` if instances of the denoted type are nullable.
  bool is_nullable;

  /// Creates an instance representing an array `element`s, nullable if `is_nullable` is `true`.
  ArrayReference(Type const& element, bool is_nullable) :
    element(element), is_nullable(is_nullable)
  {}

  ArrayReference() = delete;

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayReference const& rhs) const = default;

};

/// The type of a heterogeneous collection of data members.
struct Struct {

  friend std::ostream& operator<<(std::ostream&, const Struct&);

  /// The types of the elements in the aggregate.
  std::vector<indirect<Type>> elements;

  /// Creates an empty struct.
  Struct() = default;

  /// Creates an instance with `elements`.
  Struct(std::vector<Type> const& elements);

  /// Creates an instance with `elements`.
  Struct(std::vector<indirect<Type>>&& elements) : elements(std::move(elements)) {}

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Struct const& rhs) const = default;

};

/// The type of a reference.
struct Reference {

  friend std::ostream& operator<<(std::ostream&, const Reference&);

  /// The name of the referred entity.
  symbol::Top name;

  /// `true` if the referenced type is exactly the type denoted by this instance.
  bool is_exact;

  /// `true` if instances of the denoted type are nullable.
  bool is_nullable;

  /// Constructs an instance with the given properties.
  Reference(symbol::Top name, bool is_exact = false, bool is_nullable = true) :
    name(name), is_exact(is_exact), is_nullable(is_nullable)
  {}

  Reference() = delete;

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Reference const& rhs) const = default;

};

/// TODO
struct Var {

  friend std::ostream& operator<<(std::ostream&, const Var&);

  indirect<Type> type;

  /// Creates an instance with the given type.
  Var(Type const& type) : type(type) {}

  Var() = delete;

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Var const& other) const = default;

};

/// The type of a function.
struct Function {

  friend std::ostream& operator<<(std::ostream&, const Function&);

  /// The types of the function's parameters.
  std::vector<indirect<Type>> parameters;

  /// The type of the function's return value.
  indirect<Type> return_value;

  /// Creates an instance representing function types from `ps` to `rv`;
  Function(std::vector<Type> const& ps, Type const& rv);

  Function() = delete;

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Function const& other) const = default;

};

} // nir::type

namespace nir {

/// The type of a NIR entity.
struct Type {
private:

  friend std::ostream& operator<<(std::ostream&, const Type&);

  /// The internal representation of a type.
  using Representation = std::variant<
    type::Predefined,
    type::Numeric,
    type::ArrayValue,
    type::ArrayReference,
    type::Struct,
    type::Reference,
    type::Var,
    type::Function
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Type(T const& w) : wrapped(w) {}

  Type(Type const& other) : wrapped(other.wrapped) {}

  Type(Type&& other) : wrapped(std::move(other.wrapped)) {}

  Type() = delete;

  ~Type() noexcept {};

  Type& operator=(Type const& other) { return *this = Type(other); }

  Type& operator=(Type&& other) {
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

  /// Returns the type of the `i`-th part of an instance of the type denoted by `this`.
  ///
  /// An exception is thrown if `i` is not a valid index in `this`.
  Type element_at(uint32_t) const;

  /// Returns the type of the part identified by `path` relative to an instance of the type denoted
  /// by `this`.
  ///
  /// An exception is thrown if `path` is not valid in `this`.
  template<std::ranges::forward_range P>
  requires std::same_as<std::ranges::range_value_t<P>, uint32_t>
  Type element_at_path(const P& path) const {
    if (path.empty()) {
      return *this;
    } else {
      return element_at(path.front())
        .element_at_path(std::ranges::subrange(path | std::views::drop(1)));
    }
  }

  /// Returns the identifier of the class corresponding to this type.
  ///
  /// An exception is thrown if the wrapped instance is not a reference.
  symbol::Top class_name() const;

  /// Returns the type of that box' contents if `this` denotes the type of a box.
  std::optional<Type> unboxed() const;

  /// Returns the normalized form of `this`.
  Type normalized() const;

  /// Returns `true` if `this` denotes a box of `t`.
  bool is_box_of(Type const& t) const;

  /// Returns `true` if `this` denotes a boxed pointer.
  bool is_pointer_box() const;

  /// Returns `true` if `this` denotes a reference type.
  ///
  /// This property holds if the corresponding type in Scala is subtype of `RefKind`.
  bool is_reference() const;

  /// Returns `true` if the size of this type is known at compile-time.
  bool has_known_size() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Type const& other) const = default;

  /// Returns The null reference type.
  inline static Type null() { return { type::Predefined::null }; }

  /// Returns the unit type.
  inline static Type unit() { return { type::Predefined::unit }; }

  /// Returns the type of pointers.
  inline static Type pointer() { return { type::Predefined::pointer }; }

  /// Returns the type of sizes.
  inline static Type size() { return { type::Predefined::size }; }

  /// Returns the vararg type.
  inline static Type vararg() { return { type::Predefined::vararg }; }

  /// Returns the nothing type.
  inline static Type nothing() { return { type::Predefined::nothing }; }

  /// Returns the virtual type.
  inline static Type virtual_() { return { type::Predefined::virtual_ }; }

  /// Returns a 1-bit unsigned integer, which corresponds to Scala's `Boolean`.
  inline static Type u1() { return { type::Numeric::integer(1, false) }; }

  /// Returns a 8-bit signed integer, which corresponds to Scala's `Byte`.
  inline static Type i8() { return { type::Numeric::integer(32, true) }; }

  /// Returns a 16-bit signed integer, which corresponds to Scala's `Short`.
  inline static Type i16() { return { type::Numeric::integer(16, true) }; }

  /// Returns a 16-bit unsigned integer, which corresponds to Scala's `Char`.
  inline static Type u16() { return { type::Numeric::integer(16, false) }; }

  /// Returns a 32-bit signed integer, which corresponds to Scala's `Int`.
  inline static Type i32() { return { type::Numeric::integer(32, true) }; }

  /// Returns a 64-bit signed integer, which corresponds to Scala's `Long`.
  inline static Type i64() { return { type::Numeric::integer(64, true) }; }

  /// Returns a 32-bit IEEE 754 single-precision float.
  inline static Type f32() { return { type::Numeric::floating_point(32) }; }

  /// Returns a 64-bit IEEE 754 double-precision float.
  inline static Type f64() { return { type::Numeric::floating_point(64) }; }

};

} // nir

namespace nir::type {

/// A trait describing the API of NIR types.
template<typename Self>
struct TypeTrait {

  /// Returns the normalized form of `self`.
  static Type normalized(Self const& self) { return self.normalized(); }

};

template<>
struct TypeTrait<Predefined> {

  static Type normalized(Predefined const& self) {
    return Type(self);
  }

};

} // nir::type

#endif
