#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <tuple>
#include <type_traits>
#include <vector>

template <uint32_t upper, uint32_t lower, typename T>
  requires std::is_unsigned_v<T>
constexpr T bit_range(T value) {
  static_assert(upper <= sizeof(T) * 8,
                "Upper is out of range for supplied type");
  static_assert(lower <= sizeof(T) * 8,
                "Lower is out of range for supplied type");
  return (value >> lower) &
         static_cast<T>(std::bitset<upper + 1 - lower>().set().to_ulong());
}
template <uint8_t bitpos, typename T> constexpr T sext(T val) {
  static_assert(bitpos <= sizeof(T) * 8,
                "Trying to sign extend beyond supplied types size");
  using signed_t = std::make_signed_t<T>;
  auto shamt = sizeof(T) * 8 - bitpos - 1;
  return static_cast<T>(static_cast<signed_t>(val) << shamt >> shamt);
}
template <typename T, typename I> T sext(I init) {
  return static_cast<T>(static_cast<std::make_signed_t<T>>(
      static_cast<std::make_signed_t<I>>(init)));
}

template <typename T> class chunked_allocator {
  struct chunk {
    std::array<T, 4096> mem;
    size_t size = 0;
  };
  std::vector<std::unique_ptr<chunk>> chunks{[]() {
    std::vector<std::unique_ptr<chunk>> vec;
    vec.emplace_back(std::make_unique<chunk>());
    return vec;
  }()};

public:
  [[nodiscard("Take the span, you don't know where the elements ended up")]]
  std::span<T> save_elements(std::span<T> elements) {
    assert(elements.size() <= std::tuple_size_v<decltype(chunk::mem)>);
    if (const auto &latest_chunk = chunks.back();
        !((latest_chunk->mem.size() - latest_chunk->size) >= elements.size())) {
      chunks.push_back(std::make_unique<chunk>());
    }
    const auto &latest_chunk = chunks.back();
    T *dest = latest_chunk->mem.data() + latest_chunk->size;
    std::memcpy(dest, elements.data(), elements.size_bytes());
    latest_chunk->size += elements.size();
    return {dest, elements.size()};
  }
};
#endif