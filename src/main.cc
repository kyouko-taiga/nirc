#include "File.hh"

#include <iostream>

int main() {
  auto f = nir::File::from_contents_of("./test.nir");
  return 0;
}