#include "AttributeSet.hh"

#include <algorithm>

namespace nir {

void AttributeSet::append(Attribute const& a) {
  append(Attribute{a});
}

void AttributeSet::append(Attribute&& a) {
  if (a.is_thin()) {
    thin |= static_cast<Thin>(a.kind());
  } else {
    auto i = std::find_if(fat.begin(), fat.end(), [&](auto const& b) {
      return b.kind() == a.kind();
    });
    if ((i == fat.end()) || (*i != a)) {
      fat.push_back(a);
    }
  }
}

std::vector<Attribute> AttributeSet::elements() const {
  std::vector<Attribute> result;
  result.reserve(size());

  for (std::size_t i = 0; i < sizeof(AttributeSet::Thin) * 8; ++i) {
    Thin k = 1 << i;
    if (thin & k) {
      result.emplace_back(static_cast<attribute::Kind>(k));
      if (result.size() == thin_size()) { break; }
    }
  }

  result.insert(result.end(), fat.begin(), fat.end());
  return result;
}

} // nir
