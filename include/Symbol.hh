#ifndef NIRC_SYMBOL_H
#define NIRC_SYMBOL_H

#include "Signature.hh"

#include <memory>
#include <optional>
#include <variant>

namespace nir::symbol {

/// A stub to introduce `null`s.
struct None final {

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(None const& rhs) const = default;

};

/// A top-level symbol.
struct Top final {

  friend std::ostream& operator<<(std::ostream&, const Top&);

  /// The identifier of the symbol.
  std::string id;

  /// Creates an instance with the given identifier.
  Top(std::string const& id) : id(id) {}

  /// Creates an instance with the given identifier.
  Top(const char* id) : id(id) {}

  Top() = delete;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Top const& rhs) const = default;

};

/// A member of some top-level symbol having its own signature.
struct Member final {

  friend std::ostream& operator<<(std::ostream&, const Member&);

  /// The owner of this symbol.
  Top top;

  /// The signature of this symbol.
  Signature signature;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Member const& rhs) const = default;

};

} // nir::symbol

namespace nir {

/// The identifier of a type or method (aka a global).
struct Symbol final {
private:

  friend std::hash<Symbol>;
  friend std::ostream& operator<<(std::ostream&, const Symbol&);

  /// The internal representation of a symbol.
  using Representation = std::variant<symbol::None, symbol::Top, symbol::Member>;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Symbol(T const& w) : wrapped(w) {}

  Symbol(Symbol const& other) : wrapped(other.wrapped) {}

  Symbol(Symbol&& other) : wrapped(std::move(other.wrapped)) {}

  Symbol() = delete;

  ~Symbol() noexcept {}

  Symbol& operator=(Symbol const& other) { return *this = Symbol(other); };

  Symbol& operator=(Symbol&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns `true` if `this` wraps an instance of `T`.
  template<typename T>
  inline bool is() const {
    return std::holds_alternative<T>(wrapped);
  }

  /// Projects `this` as an instance of `T` or returns `std::nullopt` if it's diffent kind of type.
  template<typename T>
  inline std::optional<T> as() const {
    return is<T>() ? std::optional(std::get<T>(wrapped)) : std::nullopt;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Symbol const& rhs) const = default;

  /// Returns the none symbol.
  inline static Symbol none() {
    return { symbol::None{} };
  }

};

} // nir

namespace std {

template<>
struct hash<nir::symbol::Top> {

  /// Returns a hash of `self`'s salient properties.
  size_t operator()(nir::symbol::Top const& self) const;

};

template<>
struct hash<nir::symbol::Member> {

  /// Returns a hash of `self`'s salient properties.
  size_t operator()(nir::symbol::Member const& self) const;

};

template<>
struct hash<nir::Symbol> {

  /// Returns a hash of `self`'s salient properties.
  std::size_t operator()(nir::Symbol const& self) const;

};

} // std


#endif
