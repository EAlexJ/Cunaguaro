#include "decoder.h"
#include "instructions.h"
#include "loader.h"
#include "machine.h"
#include "traps.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <print>

#include <unistd.h>
using decoder = lut_decoder::Decoder;
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::println("Requires path to binary as second argument");
    return 1;
  }
  std::filesystem::path path{argv[1]};

  SimulationCtx ctx{};
  MachineState state(ctx);

  auto &PC = state.PC;

  std::map<std::string, uint64_t> syms{};
  PC = load_elf_file(state.mem, path, syms);

  if (auto it = syms.find("tohost"); it != syms.end())
    state.mem.set_tohost_addr(it->second);

  uint64_t instr_count{};
  auto start = std::chrono::steady_clock::now();
  while (!ctx.should_exit) {
    try {
      // Fetch
      // TODO: wrap fetch so Instruction_access_fault can be thrown on error
      uint32_t instr_word = state.mem.read<uint32_t>(PC);
      // Decode
      auto idx = decoder::decode(instr_word);
      //  Execute
      instruction_functions.at(idx)(state, instr_word);
      state.registers[0] = 0; // Force x0 to 0
      instr_count++;
      state.csrs.mcycle++;
      state.csrs.minstret++;
    } catch (rv_traps::Traps &trap) {
      std::println("Terminating due to trap: {}", rv_traps::get_trap_str(trap));
      ctx.exit_code = 1;
      break;
    }
  }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  double seconds = elapsed.count();
  double mips = (static_cast<double>(instr_count) / 1e6) / elapsed.count();
  std::println("\n--- Performance ---");
  std::println("Instructions: {}", instr_count);
  std::println("Host Time:    {:8.4} s", seconds);
  std::println("Throughput:   {:.6} MIPS", mips);

  return ctx.exit_code;
}