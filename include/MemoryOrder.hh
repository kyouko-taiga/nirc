#ifndef NIRC_MEMORY_ORDER_H
#define NIRC_MEMORY_ORDER_H

namespace nir {

/// An atomic memory ordering constraints.
///
/// Atomic instructions take ordering parameters specifying with which other instructions they
/// synchronize.
///
/// See also https://llvm.org/docs/LangRef.html#atomic-memory-ordering-constraints.
enum struct MemoryOrder {

  /// The set of values that can be read is governed by the happens-before partial order.
  unordered = 0,

  /// In addition to the guarantees of `unordered`, there is a single total order for modifications
  /// by monotonic operations on each address.
  monotonic = 1,

  /// In addition to the guarantees of `monotonic`, a *synchronizes-with* edge may be formed with a
  /// release operation.
  acquire = 2,

  /// In addition to the guarantees of `monotonic`, if this operation writes a value which is
  /// subsequently read by an acquire operation, it *synchronizes-with* that operation.
  release = 3,

  /// Acts as both an `acquire` and `release` operation on its address.
  acquire_release = 4,

  /// In addition to the guarantees of `acquire_release`, there is a global total order on all
  /// sequentially-consistent operations on all addresses, which is consistent with the
  /// *happens-before* partial order and with the modification orders of all the affected
  /// addresses.
  sequentially_consistent = 5

};

} // nir

#endif
