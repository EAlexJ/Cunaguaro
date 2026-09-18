#ifndef HTIF_H
#define HTIF_H
#include "memory.h"
#include "simulation_ctx.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <print>
#include <span>
#include <type_traits>
#include <unistd.h>

enum class htif_devs : uint8_t { Syscall = 0, Console = 1, Disk = 2 };
enum class htif_cmds : uint8_t { Read = 0, Write = 1 };

template <MemoryTarget BackingMem> class HtifMemoryDecorator {
  SimulationCtx &ctx_;
  BackingMem backing;

  uint64_t tohost_addr{};
  uint64_t tohost_val;

public:
  explicit HtifMemoryDecorator(SimulationCtx &ctx, BackingMem mem)
      : ctx_(ctx), backing(std::move(mem)) {};

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  T read(uint64_t addr) const {
    return backing.template read<T>(addr);
  };

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  void write(uint64_t addr, T val) {
    if constexpr (sizeof(T) == 4) {
      if (tohost_addr && addr == tohost_addr) [[unlikely]] {
        tohost_val = (tohost_val & ~std::numeric_limits<T>::max()) | val;
        return;
      } else if (tohost_addr && addr == tohost_addr + 4) [[unlikely]] {
        tohost_val = static_cast<uint64_t>(val) << 32 |
                     static_cast<uint32_t>(tohost_val);
        handle_tohost();
        return;
      }
    }
    backing.template write<T>(addr, val);
  };

  void set_tohost_addr(uint64_t th_addr) { tohost_addr = th_addr; }
  void read(uint64_t addr, std::span<std::byte> content) const {
    backing.read(addr, content);
  }
  void write(uint64_t addr, std::span<const std::byte> content) {
    backing.write(addr, content);
  }
  void handle_tohost() {
    auto dev = static_cast<htif_devs>(tohost_val >> 56 & 0xFF);
    auto cmd = static_cast<htif_cmds>(tohost_val >> 48 & 0xFF);
    switch (dev) {
      using enum htif_devs;
    case Syscall: {
      assert(cmd == static_cast<htif_cmds>(0));
      if (tohost_val & 1) {
        std::println("tohost value is 0x{:x} ({}), ending simulation",
                     tohost_val, tohost_val >> 1);
        ctx_.exit_code = static_cast<int>(tohost_val >> 1);
        ctx_.should_exit = true;
        return;
      }
      std::array<uint64_t, 8> payload{};
      try {
        backing.read(tohost_val, std::as_writable_bytes(std::span(payload)));
      } catch (std::out_of_range) {
        throw std::runtime_error("reading tohost payload went wrong");
      }
      do_htif_syscall(payload);
      return;
    }
    case Console: {
      do_htif_console(cmd);
      return;
    }
    case Disk:
      [[fallthrough]];
    default:
      throw std::runtime_error("HTIF Syscall not supported");
    }
  }
  void do_htif_syscall(std::span<uint64_t, 8> payload) {
    auto syscall = payload[0];
    if (syscall == 64) {
      auto fd = static_cast<int>(payload[1]);
      auto ptr = payload[2];
      auto len = payload[3];
      std::vector<std::byte> buf(len);
      try {
        backing.read(ptr, std::as_writable_bytes(std::span(buf)));
      } catch (std::out_of_range) {
        throw std::runtime_error("reading tohost SYS_WRITE content went wrong");
      }
      ::write(fd, buf.data(), buf.size());
    } else
      throw std::runtime_error("HTIF Syscall not supported");
    return;
  }
  void do_htif_console(auto cmd) {
    switch (cmd) {
      using enum htif_cmds;
    case Write: {
      putchar(tohost_val & 0xFF);
      return;
    }
    case Read:
      [[fallthrough]];
    default:
      throw std::runtime_error("HTIF Syscall not supported");
    }
  }
};

#endif