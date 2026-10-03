#ifndef MEMORY_H
#define MEMORY_H

#include <concepts>
#include <cstdint>
#include <span>

template <typename T>
concept MemoryTarget =
    requires(T mem, uint64_t addr, std::span<std::byte> rd_content,
             std::span<const std::byte> wr_content, uint32_t val) {
      { mem.write(addr, wr_content) } -> std::same_as<void>;
      { mem.read(addr, rd_content) } -> std::same_as<void>;
      { mem.template read<uint32_t>(addr) } -> std::same_as<uint32_t>;
      { mem.template write<uint32_t>(addr, val) } -> std::same_as<void>;
    };

#endif