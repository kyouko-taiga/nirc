#ifndef NIRC_INSTRUCTION_H
#define NIRC_INSTRUCTION_H

#include "LinktimeCondition.hh"
#include "Local.hh"
#include "Next.hh"
#include "Operation.hh"
#include "Positioned.hh"
#include "Scope.hh"
#include "SourcePosition.hh"
#include "Value.hh"

#include <variant>
#include <vector>

namespace nir {

struct Instruction;

} // nir

namespace nir::instruction {

/// A trait describing the API of NIR instructions.
template<typename T>
struct InstructionTrait {

  /// Returns the source position to which `self` corresponds.
  static SourcePosition position(T const& self) { return self.position; }

};

// TODO: Is this a basic block?
/// A label identifying a program point.
struct Label final {

  /// The identifier of the label.
  Local id;

  /// The parameters of the label.
  std::vector<value::Local> parameters;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Label const& rhs) const = default;

};

/// The declaration of a local.
struct Let final {

  /// The identifier of the local being declared.
  Local id;

  /// The operation producing the local's value.
  Operation operation;

  /// TODO
  Next next;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// TODO
  ScopeIdentifier scope;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Let const& rhs) const = default;

};

/// A return statement.
struct Return final {

  /// The return value.
  Value value;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Return const& rhs) const = default;

};

/// A jump statement.
struct Jump final {

  /// The target of the jump.
  Next target;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Jump const& rhs) const = default;

};

/// A conditional jump.
struct If final {

  /// The condition to test.
  Value condition;

  /// The target of the jump if the condition holds.
  Next success;

  /// The target of the jump if the condition does not hold.
  Next failure;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(If const& rhs) const = default;

};

/// A switch statement.
struct Switch final {

  /// The value determining the target of the switch.
  Value value;

  /// The targets of the switch, with the default case first.
  std::vector<Next> targets;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Switch const& rhs) const = default;

};

/// The throwing of an exception.
struct Throw final {

  /// The exception being thrown.
  Value exception;

  /// The target of unwinding.
  Next unwind;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Throw const& rhs) const = default;

};

/// An unreachable statement.
struct Unreachable final {

  /// TODO
  Next unwind;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Unreachable const& rhs) const = default;

};

/// A conditional jump at link-time.
struct LinktimeJump final {

  /// The condition to test.
  LinktimeCondition condition;

  /// The target of the jump if the condition holds.
  Next success;

  /// The target of the jump if the condition does not hold.
  Next failure;

  /// The source position to which this instruction corresponds.
  SourcePosition position;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(LinktimeJump const& rhs) const = default;

};

} // nir::instruction

namespace nir {

/// A NIR instruction.
struct Instruction final {
private:

  friend Positioned<Instruction>;

  /// The internal representation of an instruction.
  using Representation = std::variant<
    instruction::Label,
    instruction::Let,
    instruction::Return,
    instruction::Jump,
    instruction::If,
    instruction::Switch,
    instruction::Throw,
    instruction::Unreachable,
    instruction::LinktimeJump
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Instruction(T const& w) : wrapped(w) {}

  Instruction(Instruction const& other) : wrapped(other.wrapped) {}

  Instruction(Instruction&& other) : wrapped(std::move(other.wrapped)) {}

  Instruction() = delete;

  ~Instruction() noexcept {}

  Instruction& operator=(Instruction const& other) { return *this = Instruction(other); }

  Instruction& operator=(Instruction&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Instruction const& rhs) const = default;

};

template<>
struct Positioned<Instruction> {

  SourcePosition position(Instruction const& self) const {
    return std::visit([](auto&& self) {
      using Self = std::decay_t<decltype(self)>;
      return instruction::InstructionTrait<Self>::position(self);
    }, self.wrapped);
  }

};

} // nir

#endif
