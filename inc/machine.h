#ifndef MACHINE_H
#define MACHINE_H
#include "csr_file.h"
#include "memory/htif_proxy.h"
#include "memory/simple_mem.h"
#include "memory/virtual_writer_proxy.h"
#include "simulation_ctx.h"
#include <array>

constexpr auto memory_offset = 0x80000000;

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
  using mem_t = SimpleMem<1024 * 512>;
  mem_t mem_target{memory_offset};

public:
  std::array<reg_t, 32> registers{};
  reg_t PC{};
  SimulationCtx &ctx;
  HtifProxy<VirtualWriterProxy<mem_t>> mem;
  SwitchCaseCSRs<reg_t> csrs{};
  explicit MachineState(SimulationCtx &ctx_)
      : ctx(ctx_), mem(this->ctx, mem_target) {};
  void simulate();
  void request_stop(int code) {
    ctx.exit_code = code;
    ctx.should_exit = true;
  }
};
#endif