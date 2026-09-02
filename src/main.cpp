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

int main() {
  constexpr auto example_file = "/Users/alex/Code/ISS/fw/hello.bin";

  std::filesystem::path path{example_file};
  auto loaded_bin = BinaryLoader(path, example_file_offset);

  MachineState state;
  auto &PC = state.PC;
  bool terminate = false;
  while (!terminate) {
    try {
      // Fetch
      uint32_t instr_word = loaded_bin[PC];
      // Decode
      auto idx = Decoder::decode(instr_word);
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