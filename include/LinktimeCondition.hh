#ifndef NIRC_LINKTIME_CONDITION_H
#define NIRC_LINKTIME_CONDITION_H

#include "Operator.hh"
#include "Value.hh"
#include "Utilities/Indirect.hh"

#include <variant>
#include <string>

namespace nir {

struct LinktimeCondition;

} // nir

namespace nir::linktime {

/// A "simple" linktime-condition, i.e., a predicate on a property.
struct SimpleCondition final {

  /// The property on which the predicate applies.
  std::string lhs;

  /// The value of the predicate.
  Value rhs;

  /// The operator of the predicate.
  ComparisonOperator relation;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(SimpleCondition const& rhs) const = default;

};

/// A "complex" linktime-condition, i.e., a conjunction or disjunction of other properties.
struct ComplexCondition final {

  /// The left-hand side condition.
  indirect<LinktimeCondition> lhs;

  /// The right-hand side condition.
  indirect<LinktimeCondition> rhs;

  /// The operator of the condition.
  LogicalOperator relation;

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(ComplexCondition const& rhs) const = default;

};

} // nir::linktime

namespace nir {

/// A condition evaluated at linktime.
struct LinktimeCondition {
private:

  /// The internal representation of a linktime condition.
  using Representation = std::variant<
    linktime::SimpleCondition,
    linktime::ComplexCondition
  >;

  /// The wrapped value.
  Representation wrapped;

public:

  /// Creates an instance wrapping `w`.
  template<typename T>
  LinktimeCondition(T const& w) : wrapped(w) {}

  LinktimeCondition(LinktimeCondition const& other) : wrapped(other.wrapped) {}

  LinktimeCondition(LinktimeCondition&& other) : wrapped(std::move(other.wrapped)) {}

  LinktimeCondition() = delete;

  ~LinktimeCondition() noexcept {}

  LinktimeCondition& operator=(LinktimeCondition const& other) {
    return *this = LinktimeCondition(other);
  }

  LinktimeCondition& operator=(LinktimeCondition&& other) {
    std::swap(wrapped, other.wrapped);
    return *this;
  }

  /// Returns `true` if this instance is equal to `rhs`.
  bool operator==(LinktimeCondition const& rhs) const = default;

};

} // nir

#endif
