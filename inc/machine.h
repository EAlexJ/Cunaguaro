#ifndef MACHINE_H
#define MACHINE_H
#include "csr_file.h"
#include "decoder.h"
#include "isa.h"
#include "memory/htif_proxy.h"
#include "memory/mmio_proxy.h"
#include "memory/simple_mem.h"
#include "simulation_ctx.h"
#include "traps.h"
#include <array>

constexpr auto memory_offset = 0x80000000;

class MachineState {
private:
  using mem_t = SimpleMem<1024 * 512>;
  mem_t mem_target{memory_offset};
  struct {
    rv_traps::Traps trap = rv_traps::Traps::None;
    reg_t mtval{};
  } pending_trap;
  void step();

public:
  std::array<reg_t, 32> registers{};
  reg_t PC{};
  SimulationCtx &ctx;
  HtifProxy<MMIOProxy<mem_t>> mem;
  cached_decoder::Decoder decoder;
  SwitchCaseCSRs<reg_t> csrs{};
  explicit MachineState(SimulationCtx &ctx_)
      : ctx(ctx_), mem(this->ctx, mem_target) {};
  void simulate();
  void request_stop(int code) {
    ctx.exit_code = code;
    ctx.should_exit = true;
  }
  void trap(rv_traps::Traps which, reg_t mtval = 0) {
    pending_trap = {which, mtval};
  };
};
#endif