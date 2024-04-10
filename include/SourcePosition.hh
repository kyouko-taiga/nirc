#ifndef NIRC_SOURCE_POSITION_H
#define NIRC_SOURCE_POSITION_H

#include <string>

namespace nir {

/// A Scala source file.
struct SourceFile {
public:

  /// A tag identifying the kind of a source file.
  enum struct Kind  {

    /// An abstract file without a phyisical location (e.g., an buffer in memory).
    virtual_,

    /// A concrete file identified by its path relative to the workspace.
    ///
    /// The associated value is a path relative to the value assigned `-sourceroot` when the
    /// source was compiled, which is typically the workspace's root directory.
    concrete

  };

private:

  /// The kind of the file.
  Kind _kind;

  /// The path of the file if `_kind` is `Kind::concrete`.
  std::string path;

public:

  /// Creates a virtual source file.
  SourceFile() : _kind(Kind::virtual_) {}

  /// Creates a concrete source file identified by its path relative to the workspace.
  SourceFile(std::string const& path) : _kind(Kind::concrete), path(path) {}

  /// Returns the kind of this file.
  inline Kind kind() const { return _kind; }

  /// Returns `true` if this instance is equal to `rhs`:
  bool operator==(SourceFile const& rhs) const = default;

};

/// A position in a source file.
struct SourcePosition {

  /// The Scala source file containing this position.
  SourceFile scala_source;

  // TODO
  // /// The NIR file coordinates used to deserialize the symbol, set during linking.
  // NirSource nir_source;

  /// The 0-based line number of this position in its source.
  std::size_t line_index;

  /// The zero-based colun number of this position in its source.
  std::size_t column_index;

  /// Returns `true` if this position is invalid.
  inline bool is_invalid() const { return *this == SourcePosition::invalid(); }

  /// Returns the 1-based line number of this position in its source.
  inline std::size_t line_number() const { return line_index + 1; }

  /// Returns the 1-based column number of this position in its source.
  inline std::size_t column_number() const { return column_index + 1; }

  /// Returns the comparison of this instance with `rhs`;
  bool operator==(SourcePosition const& rhs) const = default;

  /// Returns an invalid position.
  inline static SourcePosition invalid() {
    return {
      SourceFile(),
      std::numeric_limits<std::size_t>::max(),
      std::numeric_limits<std::size_t>::max()
    };
  }

};

} // nir


#endif
