#ifndef MACHINE_H
#define MACHINE_H
#include "csr_file.h"
#include "htif.h"
#include "memory.h"
#include "simulation_ctx.h"
#include <array>

constexpr auto memory_offset = 0x80000000;
static int terminate = false;

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
constexpr unsigned IALIGN = 32;
constexpr unsigned instruction_count = 47;

using reg_t = XlenStruct<XLEN>::type;

class MachineState {
private:
  SimpleMem<1024 * 512> mem_target{memory_offset};

public:
  std::array<reg_t, 32> registers{};
  reg_t PC{};
  HtifMemoryDecorator<decltype(mem_target)> mem;
  SwitchCaseCSRs<reg_t> csrs{};
  explicit MachineState(SimulationCtx &ctx) : mem(ctx, mem_target) {};
};
#endif