#include "decoder.h"
#include "instructions.h"
#include "loader.h"
#include "machine.h"
#include "traps.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <print>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::println("Requires path to binary as second argument");
    return 1;
  }
  std::filesystem::path path{argv[1]};

  MachineState state;
  auto &PC = state.PC;
  bool terminate = false;

  PC = load_elf_file(state.mem, path);

  while (!terminate) {
    try {
      // Fetch
      // TODO: wrap fetch so Instruction_access_fault can be thrown on error
      uint32_t instr_word = state.mem.read<uint32_t>(PC);
      // Decode
      auto idx = Decoder::decode(instr_word);
      std::println("Decoded 0x{:x}: {}", state.PC, instructions.at(idx).name);
      // Execute
      instruction_functions.at(idx)(state, instr_word);
      state.registers[0] = 0; // Force x0 to 0
    } catch (rv_traps::Traps &trap) {
      std::println("Terminating due to trap: {}", rv_traps::get_trap_str(trap));
      std::terminate();
    }
  }

  return 0;
}