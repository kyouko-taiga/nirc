#ifndef NIRC_FILE_H
#define NIRC_FILE_H

#include "Attribute.hh"
#include "Decoder.hh"
#include "Definition.hh"
#include "Instruction.hh"
#include "Local.hh"
#include "Next.hh"
#include "Operation.hh"
#include "Operator.hh"
#include "Scope.hh"
#include "SourcePosition.hh"
#include "Symbol.hh"
#include "Type.hh"
#include "Tags.hh"
#include "Value.hh"

#include <any>
#include <concepts>
#include <cstdlib>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nir {

/// A value identifying serialized NIR files.
///
/// The first 4 bytes of a serialized NIR file represent a 32-bit integer equal to this value,
/// written in big-endian.
inline constexpr int32_t file_identifier = 0x2e4e4952;

/// The header of a serialized NIR file.
struct Header {

  /// The major version number of this file.
  const int compatibility_level;

  /// The minor version number of the file.
  const int revision;

  /// `true` if the file has entry pointers.
  const bool has_entry_points;

  /// Parses an instance from `source`, throwing an exception if that fails.
  static Header decode(Decoder& source);

};

/// A NIR file.
struct File {
public:

  /// The header of the file.
  Header header;

  /// The definitions in the file.
  std::vector<Definition> definitions;

  /// Creates an instance reading its contents from the file at `path`.
  static File from_contents_of(std::string const& path);

};

} // nir

#endif
