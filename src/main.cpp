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

class LoadedRiscVFile : BinaryLoader {
public:
  LoadedRiscVFile(std::filesystem::path path, unsigned start_offset = 0)
      : BinaryLoader(path, start_offset) {}
  template <typename T>
  [[nodiscard]]
  constexpr T fetch(size_t addr) {
    try {
      return this->at<T>(addr);
    } catch (std::out_of_range) {
      throw rv_traps::Traps::Instruction_access_fault;
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::println("Requires path to binary as second argument");
    return 1;
  }
  std::filesystem::path path{argv[1]};
  auto loaded_bin = LoadedRiscVFile(path, example_file_offset);

  MachineState state;
  auto &PC = state.PC;
  bool terminate = false;

  while (!terminate) {
    try {
      // Fetch
      uint32_t instr_word = loaded_bin.fetch<uint32_t>(PC);
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