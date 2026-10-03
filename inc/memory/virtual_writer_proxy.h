#ifndef VIRTUAL_WRITER_PROXY_H
#define VIRTUAL_WRITER_PROXY_H

#include "memory.h"
#include <cstdint>
#include <cstdio>

template <MemoryTarget BackingMem> class VirtualWriterProxy {
  BackingMem backing;

public:
  VirtualWriterProxy(BackingMem backing_) : backing(std::move(backing_)) {};

  template <typename T> T read(uint64_t addr) const {
    return backing.template read<T>(addr);
  };
  template <typename T> void write(uint64_t addr, T val) {
    return backing.template write<T>(addr, val);
  };

  template <> void write<uint8_t>(uint64_t addr, uint8_t val) {
    static constexpr unsigned virtual_writer_addr = 0x10000000U;
    [[unlikely]]
    if (addr == virtual_writer_addr)
      putc(val, stdout);
    else
      backing.template write<uint8_t>(addr, val);
  }
  void read(uint64_t addr, std::span<std::byte> content) const {
    backing.read(addr, content);
  }
  void write(uint64_t addr, std::span<const std::byte> content) {
    backing.write(addr, content);
  }
};
#endif