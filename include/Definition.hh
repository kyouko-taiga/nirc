#ifndef NIRC_DEFINITION_H
#define NIRC_DEFINITION_H

#include "AttributeSet.hh"
#include "Instruction.hh"
#include "Scope.hh"
#include "Symbol.hh"
#include "Type.hh"
#include "Value.hh"

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace nir::definition {

/// A trait describing the API of NIR definitions.
template<typename T>
struct DefinitionTrait {

  /// Returns the source position to which `self` corresponds.
  static SourcePosition position(T const& self) { return self.position; }

};

/// A variable or constant definition corresponding to either a field in class or module, or to a
/// top-level global variable.
///
/// - Note: this variant corresponds to `nir.Defn.Var` and `nir.Defn.Const` in the Scala codebase.
struct Binding final {

  /// The attributes of the the definition.
  AttributeSet attributes;

  /// The name of the variable being defined.
  symbol::Member name;

  /// The type of the definition.
  Type type;

  /// The value initializing the variable being defined.
  Value initializer;

  /// `true` if the binding is constant.
  bool is_constant;

  /// The source position to which this definition corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Binding const&) const = default;

};

/// A method declaration.
///
/// - Note: this variant corresponds to `nir.Defn.Declare` in the Scala codebase.
struct Forward final {

  /// The attributes of the the definition.
  AttributeSet attributes;

  /// The name of the method being declared.
  symbol::Member name;

  /// The type of the definition.
  type::Function type;

  /// The source position to which this definition corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Forward const&) const = default;

};

/// A method definition.
///
/// - Note: this variant corresponds to `nir.Defn.Define` in the Scala codebase.
struct Method final {

  /// A set of metadata about a definition for debugging purposes.
  struct DebugInformation final {

    /// A map from a local to its name.
    std::unordered_map<Local, std::string> local_name;

    /// TODO
    std::vector<LexicalScope> scopes;

    /// Returns `true` if this instance is equal to `rhs`.
    bool operator==(DebugInformation const&) const = default;

  };

  /// The attributes of the the definition.
  AttributeSet attributes;

  /// The name of the method being defined.
  symbol::Member name;

  /// The type of the definition.
  type::Function type;

  /// The body of the method.
  std::vector<Instruction> instructions;

  /// The debug information attached to the definition.
  DebugInformation debug;

  /// The source position to which this definition corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Method const&) const = default;

};

/// A trait definition.
struct Trait final {

  /// The attributes of the the definition.
  AttributeSet attributes;

  /// The name of the trait being defined.
  symbol::Top name;

  /// The traits extended by the trait being defined.
  std::vector<symbol::Top> bases;

  /// The source position to which this definition corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Trait const&) const = default;

};

/// A class definition.
struct Class final {

  /// The attributes of the the definition.
  AttributeSet attributes;

  /// The name of the class being defined.
  symbol::Top name;

  /// The superclass of the class being defined.
  std::optional<symbol::Top> parent;

  /// The traits extended by the class being defined
  std::vector<symbol::Top> traits;

  /// The source position to which this definition corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Class const&) const = default;

};

/// A module definition.
struct Module final {

  /// The attributes of the the definition.
  AttributeSet attributes;

  /// The name of the module being defined.
  symbol::Top name;

  /// TODO
  std::optional<symbol::Top> parent;

  /// TODO
  std::vector<symbol::Top> traits;

  /// The source position to which this definition corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Module const&) const = default;

};

} // nir::definition

namespace nir {

/// A class, module, trait, global variable, constant, field, or method.
struct Definition final {
private:

  friend Positioned<Definition>;

  /// The internal representation of a definition.
  using Representation = std::variant<
    definition::Binding,
    definition::Forward,
    definition::Method,
    definition::Trait,
    definition::Class,
    definition::Module
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Definition(T const& w) : wrapped(w) {}

  Definition(Definition const& other) : wrapped(other.wrapped) {}

  Definition(Definition&& other) : wrapped(std::move(other.wrapped)) {}

  Definition() = delete;

  ~Definition() noexcept {}

  Definition& operator=(Definition const& other) { return *this = Definition(other); }

  Definition& operator=(Definition&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Definition const& rhs) const = default;

};

template<>
struct Positioned<Definition> {

  SourcePosition position(Definition const& self) const {
    return std::visit([](auto&& self) {
      using T = std::decay_t<decltype(self)>;
      return definition::DefinitionTrait<T>::position(self);
    }, self.wrapped);
  }

};

} // nir

#endif
