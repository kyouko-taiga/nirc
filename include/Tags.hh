#ifndef NIRC_TAG_H
#define NIRC_TAG_H

// Note: The values of these tags are defined in `nir/serialization/Tags.scala`.

namespace nir {

/// Returns the raw value of a scoped enum of type `T`.
template<typename T>
inline constexpr std::underlying_type<T>::type raw_value(T tag) {
  return static_cast<std::underlying_type<T>::type>(tag);
}

} // nir

namespace nir::tag {

enum struct Attribute {

  may_inline = 0,
  inline_hint,
  no_inline,
  always_inline,
  may_specialize,
  no_specialize,
  un_opt,
  no_opt,
  did_opt,
  bail_opt,
  extern_,
  link,
  dyn,
  stub,
  abstract,
  volatile_,
  final,
  safe_publish,
  link_time_resolved,
  uses_intrinsic,
  align,
  define,

};

enum struct Definition {

  variable = 0,
  constant,
  declare,
  define,
  trait,
  class_,
  module,

};

enum struct Instruction {

  label = 0,
  let,
  unwind,
  return_,
  jump,
  if_,
  switch_,
  throw_,
  unreachable,
  linktime_if,

};

enum struct LinktimeCondition {

  simple = 0,
  complex,

};

enum struct BinaryOperator {

  iadd = 0,
  fadd,
  isub,
  fsub,
  imul,
  fmul,
  sdiv,
  udiv,
  fdiv,
  srem,
  urem,
  frem,
  shl,
  lshr,
  ashr,
  and_,
  or_,
  xor_,

};

enum struct ComparisonOperator {

  ieq = 0,
  ine,
  ugt,
  uge,
  ult,
  ule,
  sgt,
  sge,
  slt,
  sle,
  feq,
  fne,
  fgt,
  fge,
  flt,
  fle,

};

enum struct ConversionOperator {

  trunc = 0,
  zext,
  sext,
  fptrunc,
  fpext,
  fptoui,
  fptosi,
  uitofp,
  sitofp,
  ptrtoint,
  inttoptr,
  bitcast,
  ssize_cast,
  zsize_cast,

};

enum struct Next {

  none = 0,
  unwind,
  case_,
  label,

};

enum struct Operation {

  call = 0,
  load,
  loadAtomic,
  store,
  storeAtomic,
  element,
  extract,
  insert,
  stackalloc,
  binary,
  compare,
  convert,
  classalloc,
  classalloc_zone,
  field,
  fieldload,
  fieldstore,
  method,
  module,
  as,
  is,
  copy,
  size_of,
  alignment_of,
  box,
  unbox,
  dynmethod,
  var,
  varload,
  varstore,
  arrayalloc,
  arrayalloc_zone,
  arrayload,
  arraystore,
  arraylength,
  fence,

};

enum struct String {

  empty = 0,
  contained,
  inserted,
  appended,

};

enum struct Symbol {

  none = 0,
  top,
  member,

};

enum struct Type {

  vararg = 0,
  boolean,
  pointer,
  char_,
  byte,
  short_,
  int_,
  long_,
  float_,
  double_,
  array_value,
  struct_value,
  function,
  null,
  nothing,
  virtual_,
  var,
  unit,
  array,
  reference,
  size,

};

enum struct Value {

  true_ = 0,
  false_,
  null,
  zero,
  char_,
  byte,
  short_,
  int_,
  long_,
  float_,
  double_,
  struct_,
  array,
  byte_string,
  local,
  symbol,
  unit,
  constant,
  string,
  virtual_,
  class_of,
  linktime_condition,
  size,

};

} // nir::tag

#endif
