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
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::println("Requires path to binary as second argument");
    return 1;
  }
  std::filesystem::path path{argv[1]};

  SimulationCtx ctx{};
  MachineState machine(ctx);

  auto &PC = machine.PC;

  std::map<std::string, uint64_t> syms{};
  PC = load_elf_file(machine.mem, path, syms);

  if (auto it = syms.find("tohost"); it != syms.end())
    machine.mem.set_tohost_addr(it->second);

  auto start = std::chrono::steady_clock::now();
  machine.simulate();
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  double seconds = elapsed.count();
  double mips = (static_cast<double>(ctx.instr_count) / 1e6) / elapsed.count();
  std::println("\n--- Performance ---");
  std::println("Instructions: {}", ctx.instr_count);
  std::println("Host Time:    {:8.4} s", seconds);
  std::println("Throughput:   {:.6} MIPS", mips);

  return ctx.exit_code;
}