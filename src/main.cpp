#include "decoder.h"
#include "instructions.h"
#include "loader.h"
#include "machine.h"
#include "traps.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <limits>
#include <print>
#include <span>
#include <stdexcept>
#include <unistd.h>
#include <vector>

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
    state.mem.register_wr_callback(it->second, [](auto &mem, auto span) {
      uint64_t tohost_val{};
      std::memcpy(&tohost_val, span.data(), span.size_bytes());
      if (tohost_val & 1) {
        std::println("tohost value is 0x{:x} ({}), ending simulation",
                     tohost_val, tohost_val >> 1);
        terminate = 1;
        return;
      }
      [[maybe_unused]]
      auto dev = tohost_val >> 56 & std::numeric_limits<uint8_t>::max();
      [[maybe_unused]]
      auto cmd = tohost_val >> 48 & std::numeric_limits<uint8_t>::max();
      assert(!dev && !cmd);
      std::array<uint64_t, 8> payload{};
      try {
        mem.read(tohost_val, std::as_writable_bytes(std::span(payload)));
      } catch (std::out_of_range) {
        throw std::runtime_error("reading tohost payload went wrong");
      }
      auto syscall = payload[0];
      if (syscall == 64) {
        auto fd = static_cast<int>(payload[1]);
        auto ptr = payload[2];
        auto len = payload[3];
        std::vector<std::byte> buf(len);
        try {
          mem.read(ptr, std::as_writable_bytes(std::span(buf)));
        } catch (std::out_of_range) {
          throw std::runtime_error(
              "reading tohost SYS_WRITE content went wrong");
        }
        write(fd, buf.data(), buf.size());
      } else
        throw std::runtime_error("an unsupported tohost syscall occured");
    });
    // tohost is a 64 bit value, so any RV32 implementation issues two 32-bit
    // writes. Make the second write legal, even if it is not a "valid" addr
    // Technically, only if this second write occurs should we evaluate the
    // payload
    if constexpr (XLEN == 32)
      state.mem.register_wr_callback(it->second + 4,
                                     [](auto &, auto) { return; });
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