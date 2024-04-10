#include "Decoder.hh"

#include <algorithm>
#include <fstream>

namespace nir {

Decoder::Decoder(std::string const& path) : position{0}, byte_order{std::endian::native} {
  std::ifstream f(path, std::ios::binary);
  if (f.fail()) { throw std::ios_base::failure("file could not be opened"); }

  // Don't skip leading whitespaces.
  f.unsetf(std::ios::skipws);

  // Get the size of the file.
  f.seekg(0, std::ios::end);
  auto byte_count = f.tellg();
  f.seekg(0, std::ios::beg);

  // Copy the bytes.
  std::istream_iterator<uint8_t> b(f), e;
  source.reserve(byte_count);
  source.insert(source.begin(), b, e);
}

/// Reads enough bytes from `source` starting at `position` to form an instance of `T`, which
/// denotes a fixed-width binary numeric, interpreting bytes with the given `byte_order`.
template<typename T>
Result<T> numeric(
  const std::vector<uint8_t>& source, std::size_t& position, std::endian byte_order
) {
  if ((position + sizeof(T)) > source.size()) {
    return Result<T>::failure(DecoderError(position, "not enough bytes"));
  }

  alignas(T) uint8_t bytes[sizeof(T)];
  for (std::size_t i = 0; i < sizeof(T); ++i) {
    bytes[i] = source[position + i];
  }

  if (byte_order != std::endian::native) {
    std::reverse(std::begin(bytes), std::end(bytes));
  }

  position += sizeof(T);
  return Result<T>::success(*static_cast<T*>(static_cast<void*>(bytes)));
}

std::optional<int8_t> Decoder::peek() {
  if (position < source.size()) {
    return source[position];
  } else {
    return std::nullopt;
  }
}

Result<uint8_t> Decoder::u8() {
  return numeric<uint8_t>(source, position, byte_order);
}

Result<int8_t> Decoder::i8() {
  return numeric<int8_t>(source, position, byte_order);
}

Result<uint32_t> Decoder::u32() {
  return numeric<uint32_t>(source, position, byte_order);
}

Result<int32_t> Decoder::i32() {
  return numeric<int32_t>(source, position, byte_order);
}

Result<float> Decoder::f32() {
  return numeric<float>(source, position, byte_order);
}

Result<double> Decoder::f64() {
  return numeric<double>(source, position, byte_order);
}

Result<int64_t> Decoder::signed_leb128() {
  int64_t value = 0;
  int64_t shift = 0;

  while (true) {
    const auto b = u8();
    if (!b.is_success()) { return fail<int64_t>("ill-formed signed LEB128"); }

    int64_t slice = b.get() & 0x7f;
    if (
      (shift == 63 && slice != 0 && slice != 0x7f) ||
      (shift > 63 && slice != (value < 0 ? 0x7f : 0x00))
    ) { return fail<int64_t>("signed LEB128 too big for a 64-bit signed integer"); }

    value |= slice << shift;
    shift += 7;

    if (((b.get() & 0x80) >> 7) == 0) {
      if ((shift < 64) && (b.get() & 0x40)) {
        return Result<int64_t>::success(value | -(1 << shift));
      } else {
        return Result<int64_t>::success(value);
      }
    }
  }
}

Result<uint64_t> Decoder::unsigned_leb128() {
  int64_t value = 0;
  int64_t shift = 0;

  while (true) {
    const auto b = u8();
    if (!b.is_success()) { return fail<uint64_t>("ill-formed unsigned LEB128"); }

    int64_t slice = b.get() & 0x7f;
    if (
      (shift == 63 && (slice << shift >> shift) != slice) ||
      (shift > 63 && slice != 0)
    ) { return fail<uint64_t>("unsigned LEB128 too big for a 64-bit unsigned integer"); }

    value += slice << shift;
    if ((b.get() >> 7) == 0) { return Result<uint64_t>::success(value); }
    shift += 7;
  }
}

Result<std::string> Decoder::nullterminated_string() {
  std::string partial_result;
  char slice[64];

  while (true) {
    for (std::size_t i = 0; i < 64; ++i) {
      if (is_empty()) { return fail<std::string>("ill-formed null-terminated string"); }
      slice[i] = source[position++];
      if (slice[i] == 0) {
        partial_result.append(slice, i);
        return Result<std::string>::success(partial_result);
      }
    }
    partial_result.append(slice, 64);
  }
}

std::size_t Decoder::bytes(std::size_t n, int8_t* o) {
  std::size_t m = 0;
  while ((m < n) && !is_empty()) { o[m++] = source[position++]; }
  return m;
}

} // nir
