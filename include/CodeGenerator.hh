#ifndef NIRC_CODE_GENERATOR_H
#define NIRC_CODE_GENERATOR_H

#include "Definition.hh"

#include <memory>
#include <string>

namespace nir {

/// Code generation from NIR to some back-end.
struct CodeGenerator {

  CodeGenerator() = default;

  CodeGenerator(CodeGenerator const&) = delete;
  CodeGenerator& operator=(CodeGenerator const&) = delete;

  virtual ~CodeGenerator() = default;

  /// Generates IR for the given definition.
  virtual void emit_definition(Definition const&) = 0;

  /// Returns an instance for constructing a module named `n`.
  static std::unique_ptr<CodeGenerator> make(std::string const& n);

};

} // nir

#endif
