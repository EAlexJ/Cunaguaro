#ifndef UTIL_H
#define UTIL_H

#include <bitset>
#include <cstdint>
#include <type_traits>

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

#endif