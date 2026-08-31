#ifndef MEMORY_H
#define MEMORY_H
#include <array>
#include <cstddef>
#include <stdexcept>
template <size_t size> struct simple_mem {
  std::array<std::byte, size> backing{};
  simple_mem<size>(const unsigned off) : offset(off){};
  template <typename T> constexpr T &try_get(size_t addr) {
    [[unlikely]]
    if (addr + offset > backing.size() || addr - offset < 0)
      throw new std::out_of_range("Trying to access simple_mem out of range");
    else
      return *static_cast<T *>(backing.data() + (addr - offset));
  }
  size_t offset;
};
#endif