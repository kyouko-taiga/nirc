#include "AttributeSet.hh"
#include "Deserializer.hh"
#include "File.hh"
#include "Utilities/Assert.hh"

#include <algorithm>
#include <concepts>
#include <unordered_map>

namespace nir {

/// If `s` is defined, returns it as an instance of `optional<U>`.
template<typename T, typename U>
std::optional<T> narrowed_optional(std::optional<U> const& s) {
  return s.has_value() ? s.value().template as<T>() : std::nullopt;
}

/// Returns `s` as an instance of `vector<U>`.
template<typename T, typename U>
std::vector<T> narrowed_sequence(std::vector<U> const& s) {
  std::vector<T> r;
  r.reserve(s.size());
  std::transform(s.begin(), s.end(), r.begin(), [](auto const& e) {
    return e.template as<T>().value();
  });
  return r;
}

Header Header::decode(Decoder& source) {
  const auto m = source.i32();
  if (m != file_identifier) {
    throw DecoderError(source.current_position(), "invalid file format");
  }

  auto major = source.i32().get();
  auto minor = source.i32().get();
  // auto has_entry_points = source.u8().get() != 0; TODO
  return Header { major, minor, true };
}

File File::from_contents_of(std::string const& path) {
  Decoder source(path);

  // Read the header.
  source.byte_order = std::endian::big;
  auto header = Header::decode(source);

  // Read the header.
  source.byte_order = std::endian::little;
  Deserializer deserializer(source);
  std::vector<Definition> definitions;
  while (!deserializer.source.is_empty()) {
    definitions.push_back(deserializer.definition());
  }

  return File{header, definitions};
}

} // nir
