#ifndef SIMPLE_MEM_H
#define SIMPLE_MEM_H
#include "general.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <type_traits>
template <size_t size> class SimpleMem {

  std::array<std::byte, size> backing{};
  const size_t offset;
  [[nodiscard]]
  constexpr bool valid_bounds(uint64_t addr, size_t size_) const {
    [[unlikely]]
    if (addr < offset || (addr - offset) + size_ > backing.size())
      return false;
    return true;
  }

public:
  SimpleMem(const unsigned off) : offset(off) {};

  [[nodiscard]] constexpr status
  read(uint64_t addr, std::span<std::byte> content) const noexcept {
    if (!valid_bounds(addr, content.size_bytes()))
      return status::Error;
    const auto eff_addr = addr - offset;
    std::memcpy(content.data(), backing.data() + eff_addr,
                content.size_bytes());
    return status::Ok;
  }
  [[nodiscard]] constexpr status
  write(uint64_t addr, std::span<const std::byte> content) noexcept {
    if (!valid_bounds(addr, content.size_bytes()))
      return status::Error;
    const auto eff_addr = addr - offset;
    std::memcpy(backing.data() + eff_addr, content.data(),
                content.size_bytes());
    return status::Ok;
  }

  template <typename T>
    requires std::is_trivial_v<T>
  [[nodiscard]] constexpr status read(uint64_t addr, T &val) const noexcept {
    return read(addr, std::as_writable_bytes(std::span{&val, 1}));
  }
  template <typename T>
    requires std::is_trivial_v<T>
  [[nodiscard]] constexpr status write(uint64_t addr, T val) noexcept {
    return write(addr, std::as_bytes(std::span{&val, 1}));
  }
};

#endif