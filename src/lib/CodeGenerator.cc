#include "CodeGenerator.hh"

#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/Module.h>

namespace nir {

/// A code generator targeting LLVM IR.
struct LLVMCodeGenerator final : public CodeGenerator {

  /// The LLVM context in which IR is being generated.
  std::unique_ptr<llvm::LLVMContext> context;

  /// The LLVM module being constructed.
  std::unique_ptr<llvm::Module> module;

  /// Creates an instance for constructing a LLVM module named `n`.
  LLVMCodeGenerator(std::string const& n) : CodeGenerator(), context(nullptr), module(nullptr) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(n, *context);
  }

  /// Generates IR for the given definition.
  void emit_definition(Definition const&) override;

  /// Generates IF for the given method definition.
  void emit_method(definition::Method const&);

  /// Dumps the contents of the module to the standard output.
  void dump();

};

void LLVMCodeGenerator::emit_definition(Definition const&) {

}

void LLVMCodeGenerator::emit_method(definition::Method const&) {

}

void LLVMCodeGenerator::dump() {
  module->print(llvm::outs(), nullptr, false, true);
}

std::unique_ptr<CodeGenerator> CodeGenerator::make(std::string const& n) {
  return std::make_unique<LLVMCodeGenerator>(n);
}

} // nir
