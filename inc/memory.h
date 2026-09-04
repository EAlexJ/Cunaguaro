#ifndef MEMORY_H
#define MEMORY_H
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <type_traits>
template <size_t size> class SimpleMem {
  std::array<std::byte, size> backing{};
  size_t offset;

public:
  SimpleMem<size>(const unsigned off) : offset(off){};

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  [[nodiscard]] T read(uint64_t addr) const {
    T val;
    if (addr < offset || addr + offset > backing.size())
      throw std::out_of_range("Trying to read SimpleMem out of bounds");
    std::memcpy(&val, backing.data() + addr, sizeof(T));
    return val;
  }
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  void write(uint64_t addr, T val) {
    if (addr < offset || addr + offset > backing.size())
      throw std::out_of_range("Trying to write SimpleMem out of bounds");
    std::memcpy(backing.data() + addr, &val, sizeof(T));
  }
};
#endif