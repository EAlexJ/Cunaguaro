#include "instructions.h"
#include "loader.h"
#include "machine.h"
#include <cstdint>
#include <filesystem>

int main() {
  constexpr auto example_file = "/Users/alex/Code/ISS/fw/hello.bin";

  std::filesystem::path path{example_file};
  auto loaded_bin = binary_loader(path, example_file_offset);

  MachineState state;
  auto &PC = state.PC;
  bool terminate = false;
  while (!terminate) {
    // fetch
    [[maybe_unused]]
    uint32_t instr_word = loaded_bin[PC];
    // decode
    // execute
  }

  return 0;
}