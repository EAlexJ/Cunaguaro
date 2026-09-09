#ifndef MEMORY_H
#define MEMORY_H
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <flat_map>
#include <span>
#include <stdexcept>

constexpr unsigned virtual_writer_addr = 0x10000000U;

template <typename T>
concept Memory = requires(T mem, uint64_t addr, std::span<std::byte> content) {
  { mem.write(addr, content) } -> std::same_as<void>;
};

template <size_t size_> class SimpleMem {
  std::array<std::byte, size_> backing{};
  size_t offset;

public:
  SimpleMem<size_>(const unsigned off) : offset(off){};
  using rd_fn_sig = void(uint64_t, std::span<std::byte>) const;
  using wr_fn_sig = void(uint64_t, std::span<const std::byte>);

  // TODO: is there a non copy-paste way to do this? e.g. boost::callable_traits
  // explicitly remove the const qualifier
  using rd_callback_t = void (*)(uint64_t, std::span<std::byte>);
  using wr_callback_t = wr_fn_sig *;

  rd_fn_sig read;
  wr_fn_sig write;
  std::flat_map<uint64_t, rd_callback_t> rd_callbacks;
  std::flat_map<uint64_t, wr_callback_t> wr_callbacks;

  template <typename T> [[nodiscard]] T read(uint64_t addr) const;
  template <typename T> void write(uint64_t addr, T val);

  // required to be a continuous_range
  constexpr auto *begin() noexcept { return backing.begin(); }
  constexpr auto *end() noexcept { return backing.end(); }
  constexpr const auto *begin() const noexcept { return backing.cbegin(); }
  constexpr const auto *end() const noexcept { return backing.cend(); }
};

template <size_t size_>
void SimpleMem<size_>::read(uint64_t addr, std::span<std::byte> content) const {
  auto eff_addr = addr - offset;
  if (addr < offset || eff_addr + content.size_bytes() > backing.size())
    throw std::out_of_range("Trying to read SimpleMem out of bounds");
  std::memcpy(content.data(), backing.data() + eff_addr, content.size_bytes());
}

template <size_t size_>
void SimpleMem<size_>::write(uint64_t addr,
                             std::span<const std::byte> content) {
  auto eff_addr = addr - offset;
  if (addr < offset || eff_addr + content.size_bytes() > backing.size())
    throw std::out_of_range("Trying to write SimpleMem out of bounds");
  std::memcpy(backing.data() + eff_addr, content.data(), content.size_bytes());
}

template <size_t size_>
template <typename T>
[[nodiscard]] T SimpleMem<size_>::read(uint64_t addr) const {
  T val;
  read(addr, std::as_writable_bytes(std::span{&val, 1}));
  return val;
}

template <size_t size_>
template <typename T>
void SimpleMem<size_>::write(uint64_t addr, T val) {
  if (addr == virtual_writer_addr) {
    putc(static_cast<int>(val), stdout);
    return;
  } else {
    write(addr, std::as_bytes(std::span{&val, 1}));
  }
}

#endif