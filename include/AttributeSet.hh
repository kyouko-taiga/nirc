#ifndef NIRC_ATTRIBUTE_SET_H
#define NIRC_ATTRIBUTE_SET_H

#include "Attribute.hh"

namespace nir {

/// An (ordered) set of attributes.
///
/// The contents of the collection is ordered as follows:
/// - thin attributes are ordered before fat attributes and appear ordered by the raw value of
///   their discriminator (see `Attribute::Kind`).
/// - fat attributes are laid out in the order in which they where inserted.
///
/// Inserting an attribute already present in the set does not change the collection's order.
struct AttributeSet {
private:

  /// The type of a bitset capable of storing thin attributes.
  using Thin = std::underlying_type<attribute::Kind>::type;

  /// A bitset specifying which thin attributes are in the list.
  Thin thin;

  /// The fat members in the list.
  std::vector<Attribute> fat;

public:

  /// Inserts `a` in the collection if it is not already contained.
  ///
  /// This method does nothing if `a` is already in the collection. Otherwise, `a` is appended at
  /// the end of the collection if it is fat or inserted at its kind position if it is thin.
  void append(Attribute const& a);

  /// Inserts `a` in the collection if it is not already contained.
  ///
  /// See also alternative variant.
  void append(Attribute&& a);

  /// Returns the number of thin attributes in the collection.
  inline std::size_t thin_size() const { return std::popcount(thin); }

  /// Returns the number of fat attributes in the collection.
  inline std::size_t fat_size() const { return fat.size(); }

  /// Returns the number of attributes (thin or fat) in the collection.
  inline std::size_t size() const { return thin_size() + fat_size(); }

  /// Returns the attributes in the collection, in order.
  std::vector<Attribute> elements() const;

  /// Creates an instance with the contents of `source`, which is a container of `Attribute`s.
  template<typename T>
  static AttributeSet contents_of(const T& source) {
    AttributeSet result;
    for (auto& a : source) { result.append(a); }
    return result;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(AttributeSet const&) const = default;

};

} // nir

#endif
