#ifndef NIRC_DESERIALIZER_H
#define NIRC_DESERIALIZER_H

#include "Attribute.hh"
#include "Decoder.hh"
#include "Definition.hh"
#include "Instruction.hh"
#include "Local.hh"
#include "MemoryOrder.hh"
#include "Next.hh"
#include "Operation.hh"
#include "Operator.hh"
#include "Scope.hh"
#include "SourcePosition.hh"
#include "Symbol.hh"
#include "Type.hh"
#include "Tags.hh"
#include "Value.hh"

#include <concepts>
#include <cstdlib>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace nir {

/// The parsing of a file's serialized source.
struct Deserializer {

  /// The source from which binary data is being read.
  Decoder& source;

  /// The interned strings that have been decoded so far.
  std::vector<std::string> interned_strings;

  /// The interned symbols that have been decoded so far.
  std::vector<Symbol> interned_symbols;

  /// The interned types that have been decoded so far.
  std::vector<Type> interned_types;

  /// The interned values that have been decoded so far.
  std::vector<Value> interned_values;

  /// Creates an instance decoding data from `source`.
  Deserializer(Decoder& source) : source(source) {};

  Deserializer() = delete;
  Deserializer(Deserializer const&) = delete;
  Deserializer(Deserializer&& other) = delete;

  Deserializer& operator=(Deserializer const&) = delete;
  Deserializer& operator=(Deserializer&&) = delete;

  /// Reads a sequence of `T`s, applying `decode` to read each element.
  template<typename T, typename F>
  requires std::invocable<F, Deserializer&>
  std::vector<T> sequence(F&& decode);

  /// Reads an optional value of type `T`, applying `decode` if it is defined.
  template<typename T, typename F>
  requires std::invocable<F, Deserializer&>
  std::optional<T> optional(F&& decode);

  /// Reads an internable value of type `T`, reading or updating the memo as necessary.
  template<typename T, typename F>
  requires std::invocable<F, Deserializer&>
  T internable(std::vector<T>& memo, F&& decode);

  /// Reads a symbol (aka a "global").
  Symbol symbol();

  /// Reads a symbol signature.
  Signature signature();

  /// Reads a definition.
  Definition definition();

  /// Reads method debug information.
  definition::Method::DebugInformation debug();

  /// Reads a map from local identifier to its name.
  std::unordered_map<Local, std::string> local_name();

  /// Reads a lexical scope.
  LexicalScope lexical_scope();

  /// Reads an instruction.
  Instruction instruction();

  /// Reads a type.
  Type type();

  /// Reads a value.
  Value value();

  /// Reads a label argument.
  value::Local label_argument();

  /// Reads a continuation.
  Next next();

  /// Reads a link-time condition.
  LinktimeCondition linktime_condition();

  /// Reads an operation.
  Operation operation();

  /// Reads a binary operator.
  BinaryOperator binary_operator();

  /// Reads a comparison operator.
  ComparisonOperator comparison_operator();

  /// Reads a conversion operator.
  ConversionOperator conversion_operator();

  /// Reads a memory order.
  MemoryOrder memory_order();

  /// Reads a local.
  Local local();

  /// Reads an attribute.
  Attribute attribute();

  /// Reads a source position.
  SourcePosition source_position();

  /// Reads a scope identifier.
  ScopeIdentifier scope_identifier();

  /// Reads a string.
  std::string string();

  /// Reads a string written inline.
  ///
  /// The lenght of the string is decoded first, as an unsigned LEB128, followed by its contents, as
  /// a buffer of UTF-8 code points.
  std::string inline_string();

  /// Reads an array of bytes.
  std::vector<value::Byte> bytes();

  /// Reads a Boolean.
  bool boolean();

  /// Reads a 32-bit unsigned integer.
  ///
  /// The value is read as an unsigned LEB128 that's truncated to fit 32 bits.
  uint32_t uint32();

};

} // nir

#endif
