#ifndef NIRC_RUNTIME_H
#define NIRC_RUNTIME_H

#include "Symbol.hh"
#include "Type.hh"

namespace nir::runtime {

  /// `java.lang.Object`.
  const type::Reference Object(symbol::Top("java.lang.Object"));

  /// `java.lang.Class`.
  const type::Reference Class(symbol::Top("java.lang.Class"));

  /// `java.lang.String`.
  const type::Reference String(symbol::Top("java.lang.String"));

  /// The type of the runtime package.
  const type::Reference Runtime(symbol::Top("scala.scalanative.runtime.package$"));

  /// The nothing type of Scala's runtime.
  const type::Reference RuntimeNothing(symbol::Top("scala.runtime.Nothing$"));

  /// The null reference type of Scala's runtime.
  const type::Reference RuntimeNull(symbol::Top("scala.runtime.Null$"));

  /// The type of a boxed pointer.
  const type::Reference BoxedPointer(symbol::Top("scala.scalanative.unsafe.Ptr"));

  /// The type of a boxed null reference.
  const type::Reference BoxedNull(symbol::Top("scala.runtime.Null$"));

  /// The type of a boxed unit.
  const type::Reference BoxedUnit(symbol::Top("scala.runtime.BoxedUnit"));

  /// The type of a boxed unit module.
  const type::Reference BoxedUnitModule(symbol::Top("scala.scalanative.runtime.BoxedUnit$"));

  const type::Reference BooleanArray(symbol::Top("scala.scalanative.runtime.BooleanArray"));
  const type::Reference CharArray(symbol::Top("scala.scalanative.runtime.CharArray"));
  const type::Reference ByteArray(symbol::Top("scala.scalanative.runtime.ByteArray"));
  const type::Reference ShortArray(symbol::Top("scala.scalanative.runtime.ShortArray"));
  const type::Reference IntArray(symbol::Top("scala.scalanative.runtime.IntArray"));
  const type::Reference LongArray(symbol::Top("scala.scalanative.runtime.LongArray"));
  const type::Reference FloatArray(symbol::Top("scala.scalanative.runtime.FloatArray"));
  const type::Reference DoubleArray(symbol::Top("scala.scalanative.runtime.DoubleArray"));
  const type::Reference ObjectArray(symbol::Top("scala.scalanative.runtime.ObjectArray"));

} // nir::runtime

#endif
