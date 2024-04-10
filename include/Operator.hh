#ifndef NIRC_OPERATOR_H
#define NIRC_OPERATOR_H

namespace nir {

/// A binary operator, for use in a NIR instruction.
///
/// Binary operators denote artihmetic and logic operations having two operands and resulting in a
/// value of the same type. They are distinct from comparison operators (e.g., less than), which
/// result in a Boolean and are modeled by `ComparisonOperator`.
enum struct BinaryOperator {

  /// Integer addition.
  iadd,
  /// Floating-point addition.
  fadd,
  /// Integer subtraction.
  isub,
  /// Floating-point subtraction.
  fsub,
  /// Integer multiplication.
  imul,
  /// Floating-point multiplication.
  fmul,
  /// Signed integer division.
  sdiv,
  /// Unsigned integer division.
  udiv,
  /// Floating-point division.
  fdiv,
  /// Signed integer modulo.
  srem,
  /// Unsigned integer modulo.
  urem,
  /// Floating-point modulo.
  frem,
  /// Left shift.
  shl,
  /// Logical right shift.
  lshr,
  /// Arithmetic right shift.
  ashr,
  /// Logical AND.
  and_,
  /// Logical OR.
  or_,
  /// Logical XOR.
  xor_

};

/// A binary operator, for use in a NIR instruction.
///
/// Comparison operators are distinct from binary operators, which denote arithmetic and logic
/// operations and are modeled by `BinaryOperator`.
enum struct ComparisonOperator {

  /// Integer equality.
  ieq,
  /// Integer inequality.
  ine,

  /// Unsigned integer greater than.
  ugt,
  /// Unsigned integer greater than or equal.
  uge,
  /// Unsigned integer less than.
  ult,
  /// Unsigned integer less than or equal.
  ule,

  /// Signed integer greater than.
  sgt,
  /// Signed integer greater than or equal.
  sge,
  /// Signed integer less than.
  slt,
  /// Signed integer less than or equal.
  sle,

  /// Floating-point equality.
  feq,
  /// Floating-point inequality.
  fne,

  /// Floating-point greater than.
  fgt,
  /// Floating-point greater than or equal.
  fge,
  /// Floating-point less than.
  flt,
  /// Floating-point less than or equal.
  fle

};

/// A conversion operator, for use in a NIR instruction.
enum struct ConversionOperator {

  /// TODO
  zsize_cast,
  /// TODO
  ssize_cast,
  /// Truncation.
  trunc,
  /// Zero-extension.
  zext,
  /// Sign-extension.
  sext,
  /// Floating-point truncation.
  fptrunc,
  /// Floating-point extension.
  fpext,
  /// Floating-point to unsigned integer conversion.
  fptoui,
  /// Floating-point to signed integer conversion.
  fptosi,
  /// Unsigned integer to floating-point conversion.
  uitofp,
  /// Signed integer to floating-point conversion.
  sitofp,
  /// Pointer to integer conversion.
  ptrtoint,
  /// Integer to pointer conversion.
  inttoptr,
  /// Bitcasting.
  bitcast

};

/// A logical operator, for use in a linktime condition.
enum LogicalOperator { and_, or_ };

} // nir

#endif
