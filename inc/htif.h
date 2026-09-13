#ifndef HTIF_H
#define HTIF_H
#include "machine.h"
#include "memory.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <print>
#include <span>
#include <unistd.h>
#include <vector>
static uint64_t global_tohost_value;

enum class htif_return { Okay, Exit, Not_Supported };
enum class htif_devs : uint8_t { Syscall = 0, Console = 1, Disk = 2 };
enum class htif_cmds : uint8_t { Read = 0, Write = 1 };

[[nodiscard]]
static htif_return exec_htif(Memory auto &mem,
                             std::span<const std::byte> span) {

  if constexpr (XLEN == 32) {
    uint32_t upper;
    std::memcpy(&upper, span.data(), span.size());
    global_tohost_value =
        (static_cast<uint64_t>(upper) << 32) |
        (global_tohost_value & std::numeric_limits<uint32_t>::max());
  } else if constexpr (XLEN == 64) {
    std::memcpy(&global_tohost_value, span.data(), span.size());
  } else {
    static_assert(false, "Found invalid XLEN in exec_htif");
  }

  auto dev = static_cast<htif_devs>(global_tohost_value >> 56 & 0xFF);
  auto cmd = static_cast<htif_cmds>(global_tohost_value >> 48 & 0xFF);
  switch (dev) {
    using enum htif_devs;
  case Syscall: {
    assert(cmd == static_cast<htif_cmds>(0));
    if (global_tohost_value & 1) {
      std::println("tohost value is 0x{:x} ({}), ending simulation",
                   global_tohost_value, global_tohost_value >> 1);
      return htif_return::Exit;
    }
    std::array<uint64_t, 8> payload{};
    try {
      mem.read(global_tohost_value, std::as_writable_bytes(std::span(payload)));
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
        throw std::runtime_error("reading tohost SYS_WRITE content went wrong");
      }
      write(fd, buf.data(), buf.size());
    } else
      return htif_return::Not_Supported;
    return htif_return::Okay;
  }
  case Console: {
    switch (cmd) {
      using enum htif_cmds;
    case Read: {
      return htif_return::Not_Supported;
    }
    case Write:
      putchar(global_tohost_value & 0xFF);
      return htif_return::Okay;
    }
  }
  case Disk:
    return htif_return::Not_Supported;
  }
}

#endif