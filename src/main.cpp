#include "decoder.h"
#include "htif.h"
#include "instructions.h"
#include "loader.h"
#include "machine.h"
#include "traps.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <print>
#include <stdexcept>
#include <unistd.h>

static bool terminate = false;
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::println("Requires path to binary as second argument");
    return 1;
  }
  std::filesystem::path path{argv[1]};

  MachineState state;
  auto &PC = state.PC;

  std::map<std::string, uint64_t> syms{};
  PC = load_elf_file(state.mem, path, syms);
  if (auto it = syms.find("tohost"); it != syms.end()) {
    // tohost is a 64 bit value, so any RV32 implementation issues two 32-bit
    // writes.
    // Trigger only when the high bits are written to (assumes these two writes
    // will only ever occur in this order)
    auto tohost_trigger = it->second;
    if constexpr (XLEN == 32) {
      tohost_trigger = it->second + 4;
      // The first (lower) write should write the lower half of the tohost value
      state.mem.register_wr_callback(it->second, [](auto &, auto span) {
        std::memcpy(&global_tohost_value, span.data(), span.size());
        return;
      });
    }
    state.mem.register_wr_callback(tohost_trigger, [](auto &mem, auto span) {
      auto ret = exec_htif(mem, span);
      switch (ret) {
        using enum htif_return;
      case Exit:
        terminate = 1;
        break;
      case Not_Supported:
        throw std::runtime_error("HTIF Syscall not supported");
      case Okay:
      default:
      }
      return;
    });
  };

  while (!terminate) {
    try {
      // Fetch
      // TODO: wrap fetch so Instruction_access_fault can be thrown on error
      uint32_t instr_word = state.mem.read<uint32_t>(PC);
      // Decode
      auto idx = Decoder::decode(instr_word);
      // Execute
      instruction_functions.at(idx)(state, instr_word);
      state.registers[0] = 0; // Force x0 to 0
    } catch (rv_traps::Traps &trap) {
      std::println("Terminating due to trap: {}", rv_traps::get_trap_str(trap));
      return 1;
    }
  }

  return 0;
}