#include "decoder.h"
#include "instructions.h"
#include "loader.h"
#include "machine.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <print>

int main() {
  constexpr auto example_file = "/Users/alex/Code/ISS/fw/hello.bin";

  std::filesystem::path path{example_file};
  auto loaded_bin = binary_loader(path, example_file_offset);

  MachineState state;
  auto &PC = state.PC;
  bool terminate = false;
  while (!terminate) {
    // Fetch
    uint32_t instr_word = loaded_bin[PC];
    // Decode
    auto idx = decoder::decode(instr_word);
    assert(idx != std::numeric_limits<size_t>::max() && "Illegal instruction");
    std::println("Decoded {}", instructions.at(idx).name);
    // Execute
    assert(idx <= instruction_functions.size() &&
           "Instruction functionality not implemented");
    instruction_functions.at(idx)(state, instr_word);
    state.registers[0] = 0; // Force x0 to 0
  }

  return 0;
}