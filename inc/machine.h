#ifndef MACHINE_H
#define MACHINE_H
#include "memory.h"
#include <array>
constexpr auto example_file_offset = 0x10080000;
constexpr auto example_mem_offset = 0x30000000;

template <size_t V>
concept valid_XLEN = requires() { V == 32 || V == 64; };
template <uint32_t XLEN>
  requires valid_XLEN<XLEN>
struct xlen_struct {};
template <> struct xlen_struct<32> {
  using type = uint32_t;
};
template <> struct xlen_struct<64> {
  using type = uint64_t;
};
constexpr auto XLEN = 32;

using reg_t = xlen_struct<XLEN>::type;

struct MachineState {
  std::array<reg_t, 32> registers{};
  reg_t PC{example_file_offset};
  simple_mem<1024 * 256> mem{example_mem_offset};
};
#endif