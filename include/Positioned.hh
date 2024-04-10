#ifndef NIRC_POSITIONED_H
#define NIRC_POSITIONED_H

#include "SourcePosition.hh"

namespace nir {

/// A trait for types describing objects having a position in source code.
template<typename Self>
struct Positioned final {

  /// Returns the position of `self` in sources.
  SourcePosition position(Self const&) const { SourcePosition::invalid(); }

};

} // nir


#endif
