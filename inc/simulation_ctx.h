#ifndef SIMULATION_CTX_H
#define SIMULATION_CTX_H
struct SimulationCtx {
  unsigned long instr_count{};
  unsigned long fetch_count{};
  int exit_code{};
  bool should_exit{};
};
#endif