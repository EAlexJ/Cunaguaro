#ifndef MEMORY_H
#define MEMORY_H

#include "general.h"
#include <concepts>
#include <cstdint>
#include <span>

template <typename T>
concept MemoryTarget =
    requires(T mem, uint64_t addr, std::span<std::byte> rd_content,
             std::span<const std::byte> wr_content, uint32_t val,
             uint32_t &val_ref) {
      { mem.write(addr, wr_content) } -> std::same_as<status>;
      { mem.read(addr, rd_content) } -> std::same_as<status>;
      { mem.template read<uint32_t>(addr, val_ref) } -> std::same_as<status>;
      { mem.template write<uint32_t>(addr, val) } -> std::same_as<status>;
    };

#endif