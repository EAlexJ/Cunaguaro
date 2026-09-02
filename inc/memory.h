#ifndef MEMORY_H
#define MEMORY_H
#include "traps.h"
#include <array>
#include <cstddef>
#include <stdexcept>
template <size_t size> struct SimpleMem {
  std::array<std::byte, size> backing{};
  SimpleMem<size>(const unsigned off) : offset(off){};
  template <typename T> constexpr T &try_get(size_t addr) {
    [[unlikely]]
    if (addr + offset > backing.size() || addr - offset < 0)
      throw std::out_of_range("Trying to access SimpleMem out of range");
    else
      return *reinterpret_cast<T *>(backing.data() + (addr - offset));
  }
  template <typename T> constexpr T fetch(size_t addr) {
    try {
      this->try_get<T>(addr);
    } catch (std::out_of_range) {
      throw rv_traps::Traps::Instruction_access_fault;
    }
  }

  size_t offset;
};
#endif