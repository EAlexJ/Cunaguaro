#ifndef MEMORY_H
#define MEMORY_H
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <type_traits>

constexpr unsigned virtual_writer_addr = 0x10000000U;
template <size_t size> class SimpleMem {
  std::array<std::byte, size> backing{};
  size_t offset;

public:
  SimpleMem<size>(const unsigned off) : offset(off){};

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  [[nodiscard]] T read(uint64_t addr) const {
    T val;
    auto eff_addr = addr - offset;
    if (addr < offset || eff_addr + sizeof(T) > backing.size())
      throw std::out_of_range("Trying to read SimpleMem out of bounds");
    std::memcpy(&val, backing.data() + eff_addr, sizeof(T));
    return val;
  }
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  void write(uint64_t addr, T val) {
    if (addr == virtual_writer_addr) {
      putc(static_cast<int>(val), stdout);
      return;
    } else {
      auto eff_addr = addr - offset;
      if (addr < offset || eff_addr + sizeof(T) > backing.size())
        throw std::out_of_range("Trying to write SimpleMem out of bounds");
      std::memcpy(backing.data() + eff_addr, &val, sizeof(T));
    }
  }
};
#endif