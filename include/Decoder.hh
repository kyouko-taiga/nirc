#ifndef NIRC_DECODER_H
#define NIRC_DECODER_H

#include "Utilities/Assert.hh"
#include "Utilities/Result.hh"

#include <bit>
#include <cstdlib>
#include <string>
#include <vector>

namespace nir {

/// An error that occurred during decoding.
struct DecoderError: public std::exception {

  /// The byte offset in the source file at which the error occured.
  const std::size_t offset;

  /// A description of the error.
  const std::string diagnostic;

  /// Creates an instance denoting a decoding error that occured at offset `p` and can be diagnosed
  /// by `d`.
  DecoderError(std::size_t p, const std::string& d) : offset(p), diagnostic(d) {}

};

/// A helper to decode information from an array of bytes.
struct Decoder {
private:

  /// The raw bytes from which information is being decoded.
  std::vector<uint8_t> source;

  /// The current position of the decoder in `source`.
  std::size_t position;

  /// Returns a decoding failure at the current position diagnosed by `d`.
  template<typename T>
  Result<T> fail(std::string const& d) const {
    return Result<T>::failure(DecoderError(position, d));
  }

public:

  /// The order in which bytes are read.
  std::endian byte_order;

  /// Creates an instance for decoding the contents of the file at `path`.
  Decoder(std::string const& path);

  /// Returns the number of bytes in the source from which data is being read.
  inline std::size_t source_size() const { return source.size(); }

  /// The current position of the decoder in its source.
  inline std::size_t current_position() const { return position; }

  /// Returns `true` if there is no more byte to read from the current position.
  inline bool is_empty() const { return current_position() == source_size(); }

  /// Moves the decoder at `p`.
  ///
  /// - Precondition: `p` is in the range [`0`, `source_size()`).
  inline void move_at(std::size_t p) {
    precondition(p < source.size(), "position is out of bounds");
    position = p;
  }

  /// Reads the next byte without consuming it.
  std::optional<int8_t> peek();

  /// Reads the next byte as a 8-bit unsigned integer.
  Result<uint8_t> u8();

  /// Reads the next byte as a 8-bit signed integer.
  Result<int8_t> i8();

  /// Reads the next byte as a 32-bit unsigned integer.
  Result<uint32_t> u32();

  /// Reads the next byte as a 32-bit signed integer.
  Result<int32_t> i32();

  /// Reads a 32-bit floating-point number.
  Result<float> f32();

  /// Reads a 64-bit floating-point number.
  Result<double> f64();

  /// Reads a signed integer in little endian base 128.
  Result<int64_t> signed_leb128();

  /// Reads an unsigned integer in little endian base 128.
  Result<uint64_t> unsigned_leb128();

  /// Reads a null-terminated string.
  Result<std::string> nullterminated_string();

  /// Reads up to `n` bytes, writes them to `o`, and returns the number of read bytes.
  ///
  /// - Precondition: `o` must be a pointer to a buffer large enough to contain `n` elements.
  std::size_t bytes(std::size_t n, int8_t* o);

};

} // nir

#endif
