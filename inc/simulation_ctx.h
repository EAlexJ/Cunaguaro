#ifndef SIMULATION_CTX_H
#define SIMULATION_CTX_H
struct SimulationCtx {
  int exit_code{};
  unsigned long instr_count{};
  bool should_exit{};
};
#endif