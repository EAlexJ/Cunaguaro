#ifndef SIMPLE_MEM_H
#define SIMPLE_MEM_H
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <stdexcept>
template <size_t size> class SimpleMem {

  std::array<std::byte, size> backing{};
  const size_t offset;

  constexpr void check_bounds(uint64_t addr, size_t size_) const {
    [[unlikely]]
    if (addr < offset || (addr - offset) + size_ > backing.size())
      throw std::out_of_range("Trying to read SimpleMem out of bounds");
  }

public:
  SimpleMem(const unsigned off) : offset(off) {};

  constexpr void read(uint64_t addr, std::span<std::byte> content) const {
    check_bounds(addr, content.size_bytes());
    const auto eff_addr = addr - offset;
    std::memcpy(content.data(), backing.data() + eff_addr,
                content.size_bytes());
  }
  constexpr void write(uint64_t addr, std::span<const std::byte> content) {
    check_bounds(addr, content.size_bytes());
    const auto eff_addr = addr - offset;
    std::memcpy(backing.data() + eff_addr, content.data(),
                content.size_bytes());
  }

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  [[nodiscard]] constexpr T read(uint64_t addr) const {
    T val;
    read(addr, std::as_writable_bytes(std::span{&val, 1}));
    return val;
  }
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  constexpr void write(uint64_t addr, T val) {
    write(addr, std::as_bytes(std::span{&val, 1}));
  }
};

#endif