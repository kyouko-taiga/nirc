#ifndef NIRC_OPERATION_H
#define NIRC_OPERATION_H

#include "MemoryOrder.hh"
#include "Operator.hh"
#include "Signature.hh"
#include "Symbol.hh"
#include "Type.hh"
#include "Value.hh"

#include <cstdlib>
#include <memory>
#include <optional>
#include <variant>

namespace nir::operation {

/// A function call.
struct Call final  {

  /// The type of the function being called.
  type::Function callee_type;

  /// The callee.
  Value callee;

  /// The arguments of the call.
  std::vector<Value> arguments;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return *callee_type.return_value; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Call const& rhs) const = default;

};

/// The loading of a value from memory.
struct Load final {

  /// The type of the value being loaded.
  Type type;

  /// The location in memory from where the value is loaded.
  Value source;

  /// An optional atomic memory ordering constraint on the operation.
  std::optional<MemoryOrder> ordering;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Load const& rhs) const = default;

};

/// The writing of a value to memory.
struct Store final {

  /// The type of the value being stored.
  Type type;

  /// The location in memory where `source` should be stored.
  Value target;

  /// The value to store.
  Value source;

  /// An optional atomic memory ordering constraint on the operation.
  std::optional<MemoryOrder> ordering;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::unit(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Store const& rhs) const = default;

};

/// The computation of the address of a part at a path relative to some aggregate.
struct Element final {

  /// The type of the object containing the part being accessed.
  Type whole_type;

  /// The object containing the part being accessed.
  Value whole;

  /// A path identifying the part being accessed.
  std::vector<uint32_t> path;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::pointer(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Element const& rhs) const = default;

};

/// The copy of a value from an aggregate.
struct Extract final {

  /// The object containing the part being copied.
  Value whole;

  /// A path identifying the value being copied.
  std::vector<uint32_t> path;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return whole.type().element_at_path(path); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Extract const& rhs) const = default;

};

/// The replacement of a part in an aggregate.
struct Insert final {

  /// The object containing the part being replaced.
  Value whole;

  /// The replacing value.
  Value part;

  /// A path identifying the value being replaced.
  std::vector<uint32_t> path;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return whole.type(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Insert const& rhs) const = default;

};

/// The allocation of memory on the stack.
struct StackAllocate final {

  /// The type of the storage being allocated.
  Type type;

  /// The number of instances for which storage is being allocated.
  std::size_t count;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::pointer(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(StackAllocate const& rhs) const = default;

};

/// The application of a binary operation.
struct BinaryApply final {

  /// The operation being applied.
  BinaryOperator callee;

  /// The type of the operands.
  Type operand_type;

  /// The left operand.
  Value lhs;

  /// The right operand.
  Value rhs;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return operand_type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(BinaryApply const& rhs) const = default;

};

/// A comparison.
struct Compare final {

  /// The operation being applied.
  ComparisonOperator callee;

  /// The type of the operands.
  Type operand_type;

  /// The left operand.
  Value lhs;

  /// The right operand.
  Value rhs;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::u1(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Compare const& rhs) const = default;

};

/// The conversion of a value from one type to another.
struct Convert final {

  /// The conversion being applied.
  ConversionOperator callee;

  /// The type to which `source` is converted.
  Type target;

  /// The value being converted.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return target; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Convert const& rhs) const = default;

};

/// The introduction of a happens-before edge between operations.
struct Fence final {

  /// The description of the edge being added.
  MemoryOrder ordering;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::unit(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Fence const& rhs) const = default;

};

/// The allocation of a class.
struct ClassAllocate final {

  /// The name of the class being loaded.
  symbol::Top name;

  /// The zone in which the class is being allocated.
  std::optional<Value> zone;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type(type::Reference{name, true, false}); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ClassAllocate const& rhs) const = default;

};

/// The loading a value stored in the field of a class instance.
struct FieldLoad final {

  /// The type of the value being loaded.
  Type type;

  /// The object containing the field whose value is being loaded.
  Value owner;

  /// The name of the field bein loaded.
  symbol::Member name;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(FieldLoad const& rhs) const = default;

};

/// The assignment of a field in a class instance.
struct FieldStore final {

  /// The type of the value being stored.
  Type type;

  /// The object containing the field whose value is being assigned.
  Value owner;

  /// The name of the field being assigned.
  symbol::Member name;

  /// The value to store.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::unit(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(FieldStore const& rhs) const = default;

};

/// The selection of a field in a class instance.
struct Field final {

  /// The object containing the field being selected.
  Value owner;

  /// The name of the field being selected.
  symbol::Member name;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::pointer(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Field const& rhs) const = default;

};

/// The selection of a method in a class instance.
struct Method final {

  /// The object containing the method is being selected.
  Value owner;

  /// The signature of the method being selected.
  Signature signature;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::pointer(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Method const& rhs) const = default;

};

/// The dynamic selection of a method in a class instance.
struct DynamicMethod final {

  /// The object containing the method is being selected.
  Value owner;

  /// The signature of the method being selected.
  Signature signature;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::pointer(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(DynamicMethod const& rhs) const = default;

};

/// The selection of a module.
struct Module final {

  /// The name of the module being selected.
  symbol::Top name;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type(type::Reference{name, true, false}); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Module const& rhs) const = default;

};

/// The conversion of a class instance from one type to another.
struct As final {

  /// The type to which `source` is converted.
  Type target;

  /// The instance being converted.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return target; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(As const& rhs) const = default;

};

/// A type test.
struct Is final {

  /// The type `source` must have for the test to succeed.
  Type target;

  /// The instance whose type is being tested.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::u1(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Is const& rhs) const = default;

};

/// The copy of a value.
struct Copy final {

  /// The value being copied.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return source.type(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Copy const& rhs) const = default;

};

/// The query of a type's size.
struct SizeOf final {

  /// The type whose size is queried.
  Type operand;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::size(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(SizeOf const& rhs) const = default;

};

/// The query of a type's alignment.
struct AlignmentOf final {

  /// The type whose alignment is queried.
  Type operand;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::size(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(AlignmentOf const& rhs) const = default;

};

/// The boxing of a value.
struct Box final {

  /// The type of the box.
  Type box_type;

  /// The value being boxed.
  Value contents;

  /// Returns the type of this operation's result.
  inline Type result_type() const {
    auto t = box_type.as<type::Reference>().value();
    auto is_nullable = box_type.is_pointer_box();
    return Type(type::Reference(box_type.class_name(), true, is_nullable));
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Box const& rhs) const = default;

};

/// The boxing of a value.
struct Unbox final {

  /// The type of the box.
  Type box_type;

  /// The box.
  Value box;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return box_type.unboxed().value(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Unbox const& rhs) const = default;

};

// TODO
struct Var final {

  Type type;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type(type::Var(type)); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Var const& rhs) const = default;

};

// TODO
struct VarLoad final {

  Value slot;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return *(slot.type().as<type::Var>().value().type); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(VarLoad const& rhs) const = default;

};

// TODO
struct VarStore final {

  Value slot;

  /// The value to store.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::unit(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(VarStore const& rhs) const = default;

};

/// The allocation of a Scala array.
struct ArrayAllocate final {

  /// The type of the storage being allocated.
  Type element;

  /// The value initializing the contents of the array being allocated.
  Value initializer;

  /// The zone in which the class is being allocated.
  std::optional<Value> zone;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type(type::ArrayReference(element, false)); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayAllocate const& rhs) const = default;

};

/// The loading a value stored in a Scala array.
struct ArrayLoad final {

  /// The type of the value being loaded.
  Type type;

  /// The array containing the value being loaded.
  Value owner;

  /// The position of the value in the array.
  uint32_t position;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return type; }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayLoad const& rhs) const = default;

};

/// The assignment of a position in a Scala array.
struct ArrayStore final {

  /// The type of the value being stored.
  Type type;

  /// The array containing the position being assigned.
  Value owner;

  /// The position of the value in the array.
  uint32_t position;

  /// The value to store.
  Value source;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::unit(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayStore const& rhs) const = default;

};

/// The query of a Scala array's length.
struct ArrayLength final {

  /// The array whose lenght is queried.
  Value operand;

  /// Returns the type of this operation's result.
  inline Type result_type() const { return Type::i32(); }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ArrayLength const& rhs) const = default;

};

} // nir::operation

namespace nir {

/// A NIR operation.
struct Operation {
private:

  /// The internal representation of an operation.
  using Representation = std::variant<
    operation::Call,
    operation::Load,
    operation::Store,
    operation::Element,
    operation::Extract,
    operation::Insert,
    operation::StackAllocate,
    operation::BinaryApply,
    operation::Compare,
    operation::Convert,
    operation::Fence,
    operation::ClassAllocate,
    operation::FieldLoad,
    operation::FieldStore,
    operation::Field,
    operation::Method,
    operation::DynamicMethod,
    operation::Module,
    operation::As,
    operation::Is,
    operation::Copy,
    operation::SizeOf,
    operation::AlignmentOf,
    operation::Box,
    operation::Unbox,
    operation::Var,
    operation::VarLoad,
    operation::VarStore,
    operation::ArrayAllocate,
    operation::ArrayLoad,
    operation::ArrayStore,
    operation::ArrayLength
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  Operation(T const& w) : wrapped(w) {}

  Operation(Operation const& other) : wrapped(other.wrapped) {}

  Operation(Operation&& other) : wrapped(std::move(other.wrapped)) {}

  Operation() = delete;

  ~Operation() noexcept {}

  Operation& operator=(Operation const& other) { return *this = Operation(other); }

  Operation& operator=(Operation&& other) {
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

  /// Returns the type of this operation's result.
  Type result_type() const;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(Operation const& rhs) const = default;

};

/// A trait describing the API of NIR instructions.
template<typename Self>
struct OperationTrait {

  /// Returns the type of this `self`'s result.
  static Type result_type(Self const& self) { return self.result_type(); }

};

} // nir

#endif
