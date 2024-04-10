#include "File.hh"
#include "CodeGenerator.hh"

#include <iostream>

int main() {
  auto f = nir::File::from_contents_of("./test.nir");
  auto g = nir::CodeGenerator::make("Test");
  return 0;
}
