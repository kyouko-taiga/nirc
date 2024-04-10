#ifndef NIRC_ATTRIBUTE_H
#define NIRC_ATTRIBUTE_H

#include <bit>
#include <cstdlib>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace nir::attribute {

enum struct Kind : uint32_t;

/// A trait describing the API of NIR attributes.
template<typename Self>
struct AttributeTrait {

  /// Returns the kind of `self`.
  static Kind kind(Self const&) { return Self::kind; }

};

/// The kind of an attribute.
///
/// These tags do not match `tags::Attribute` to support a bitset representation.
enum struct Kind : uint32_t {

  may_inline          = 1,
  inline_hint         = 1 << 1,
  no_inline           = 1 << 2,
  always_inline       = 1 << 3,
  may_specialize      = 1 << 4,
  no_specialize       = 1 << 5,
  un_opt              = 1 << 6,
  no_opt              = 1 << 7,
  did_opt             = 1 << 8,
  bail_opt            = 1 << 9,
  dyn                 = 1 << 10,
  stub                = 1 << 11,
  extern_             = 1 << 12,
  link                = 1 << 13,
  define              = 1 << 14,
  abstract            = 1 << 15,
  volatile_           = 1 << 16,
  final               = 1 << 17,
  safe_publish        = 1 << 18,
  link_time_resolved  = 1 << 19,
  uses_intrinsic      = 1 << 20,
  alignment           = 1 << 21

};

template<>
struct AttributeTrait<Kind> {
  static Kind kind(Kind const& self) { return self; }
};

std::ostream& operator<<(std::ostream&, Kind);

/// TODO
struct BailOpt final {

  friend std::ostream& operator<<(std::ostream&, BailOpt const&);

  static constexpr Kind kind = Kind::bail_opt;

  std::string message;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(BailOpt const& rhs) const = default;

};

/// TODO
struct Extern final {

  friend std::ostream& operator<<(std::ostream&, Extern const&);

  static constexpr Kind kind = Kind::extern_;

  bool is_blocking;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Extern const& rhs) const = default;

};

/// TODO
struct Link final {

  friend std::ostream& operator<<(std::ostream&, Link const&);

  static constexpr Kind kind = Kind::link;

  std::string name;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Link const& rhs) const = default;

};

/// TODO
struct Define final {

  friend std::ostream& operator<<(std::ostream&, Define const&);

  static constexpr Kind kind = Kind::define;

  std::string name;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Define const& rhs) const = default;

};

/// TODO
struct Alignment final {

  friend std::ostream& operator<<(std::ostream&, Alignment const&);

  static constexpr Kind kind = Kind::alignment;

  int64_t size;

  std::optional<std::string> group;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Alignment const& rhs) const = default;

};

} // nir::attribute

namespace nir {

/// An attribute that can be attached to a NIR definition.
///
/// There are two sorts of attributes: thin and fat. Thin attributes are merely flags and can be
/// represented efficiently using a bitset. Fat attributes have associate values.
struct Attribute {
private:

  friend std::ostream& operator<<(std::ostream&, Attribute const&);

  /// The internal representation of a symbol.
  using Representation = std::variant<
    attribute::Kind,
    attribute::BailOpt,
    attribute::Extern,
    attribute::Link,
    attribute::Define,
    attribute::Alignment
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Attribute(T const& w) : wrapped(w) {}

  Attribute(Attribute const& other) : wrapped(other.wrapped) {}

  Attribute(Attribute&& other) : wrapped(std::move(other.wrapped)) {}

  Attribute() = delete;

  ~Attribute() noexcept {}

  Attribute& operator=(Attribute const& other) { return *this = Attribute(other); };

  Attribute& operator=(Attribute&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns the kind of this attribute.
  attribute::Kind kind() const;

  /// Returns `true` if this attribute does not have any associated value.
  inline bool is_thin() const { return wrapped.index() == 0; }

  /// Returns `true` if `this` wraps an instance of `T`.
  template<typename T>
  inline bool is() const {
    return std::holds_alternative<T>(wrapped);
  }

  /// Returns `true` if `this` wraps an attribute with kind `k`.
  inline bool is(attribute::Kind k) const {
    return kind() == k;
  }

  /// Projects `this` as an instance of `T` or returns `std::nullopt` if it's diffent kind of type.
  template<typename T>
  inline std::optional<T> as() const {
    return is<T>() ? std::optional(std::get<T>(wrapped)) : std::nullopt;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Attribute const&) const = default;

};

} // nir

#endif
