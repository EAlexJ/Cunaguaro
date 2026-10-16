#ifndef ISA_H
#define ISA_H
#include <cstddef>
#include <cstdint>
template <size_t V>
concept valid_XLEN = requires() { V == 32 || V == 64; };
template <uint32_t XLEN>
  requires valid_XLEN<XLEN>
struct XlenStruct {};
template <> struct XlenStruct<32> {
  using type = uint32_t;
};
template <> struct XlenStruct<64> {
  using type = uint64_t;
};

constexpr auto XLEN = 32;
using reg_t = XlenStruct<XLEN>::type;

constexpr unsigned IALIGN = 32;

constexpr unsigned instruction_count = 47;

#endif