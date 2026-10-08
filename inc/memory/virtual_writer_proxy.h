#ifndef VIRTUAL_WRITER_PROXY_H
#define VIRTUAL_WRITER_PROXY_H

#include "general.h"
#include "memory.h"
#include <cstdint>
#include <cstdio>

template <MemoryTarget BackingMem> class VirtualWriterProxy {
  BackingMem backing;

public:
  VirtualWriterProxy(BackingMem backing_) : backing(std::move(backing_)) {};
  template <typename T>
  [[nodiscard]]
  status read(uint64_t addr, T &val) const {
    return backing.template read<T>(addr, val);
  };
  template <typename T>
  [[nodiscard]]
  status write(uint64_t addr, T val) {
    if constexpr (std::is_same_v<T, uint8_t>) {
      static constexpr unsigned virtual_writer_addr = 0x10000000U;
      [[unlikely]]
      if (addr == virtual_writer_addr) {
        putc(val, stdout);
        return status::Ok;
      } else
        return backing.template write<uint8_t>(addr, val);
    } else
      return backing.template write<T>(addr, val);
  };

  [[nodiscard]]
  status read(uint64_t addr, std::span<std::byte> content) const {
    return backing.read(addr, content);
  }
  [[nodiscard]]
  status write(uint64_t addr, std::span<const std::byte> content) {
    return backing.write(addr, content);
  }
};
#endif