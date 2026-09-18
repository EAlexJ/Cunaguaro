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
#include <utility>

template <typename T>
concept MemoryTarget =
    requires(T mem, uint64_t addr, std::span<std::byte> rd_content,
             std::span<const std::byte> wr_content, uint32_t val) {
      { mem.write(addr, wr_content) } -> std::same_as<void>;
      { mem.read(addr, rd_content) } -> std::same_as<void>;
      { mem.template read<uint32_t>(addr) } -> std::same_as<uint32_t>;
      { mem.template write<uint32_t>(addr, val) } -> std::same_as<void>;
    };

template <size_t size> class SimpleMem {

  std::array<std::byte, size> backing{};
  size_t offset;

  using rd_callback_t = void (*)(const SimpleMem<size> &, std::span<std::byte>);
  using wr_callback_t = void (*)(SimpleMem<size> &, std::span<const std::byte>);
  std::flat_map<uint64_t, rd_callback_t> rd_callbacks;
  std::flat_map<uint64_t, wr_callback_t> wr_callbacks;

public:
  SimpleMem<size>(const unsigned off) : offset(off) {
    constexpr unsigned virtual_writer_addr = 0x10000000U;
    register_wr_callback(
        virtual_writer_addr, [](auto &_, std::span<const std::byte> content) {
          assert(content.size_bytes() == 1);
          char value;
          std::memcpy(&value, content.data(), content.size_bytes());
          putc(value, stdout);
          return;
        });
  };

  void read(uint64_t, std::span<std::byte>) const;
  void write(uint64_t, std::span<const std::byte>);

  template <typename T> [[nodiscard]] T read(uint64_t addr) const;
  template <typename T> void write(uint64_t addr, T val);
  constexpr void register_rd_callback(uint64_t addr, rd_callback_t fn) {
    rd_callbacks.emplace(addr, std::move(fn));
  }
  constexpr void register_wr_callback(uint64_t addr, wr_callback_t fn) {
    wr_callbacks.emplace(addr, std::move(fn));
  }
};

template <size_t size>
void SimpleMem<size>::read(uint64_t addr, std::span<std::byte> content) const {
  if (!rd_callbacks.empty()) [[unlikely]] {
    if (auto it = rd_callbacks.find(addr); it != rd_callbacks.end()) {
      it->second(*this, content);
      return;
    }
  }
  [[unlikely]]
  if (addr < offset || (addr - offset) + content.size_bytes() > backing.size())
    throw std::out_of_range("Trying to read SimpleMem out of bounds");
  const auto eff_addr = addr - offset;
  std::memcpy(content.data(), backing.data() + eff_addr, content.size_bytes());
}

template <size_t size>
void SimpleMem<size>::write(uint64_t addr, std::span<const std::byte> content) {
  if (!wr_callbacks.empty()) {
    if (auto it = wr_callbacks.find(addr); it != wr_callbacks.end()) {
      it->second(*this, content);
      return;
    }
  }
  [[unlikely]]
  if (addr < offset || (addr - offset) + content.size_bytes() > backing.size())
    throw std::out_of_range("Trying to write SimpleMem out of bounds");
  const auto eff_addr = addr - offset;
  std::memcpy(backing.data() + eff_addr, content.data(), content.size_bytes());
}

template <size_t size>
template <typename T>
[[nodiscard]] T SimpleMem<size>::read(uint64_t addr) const {
  T val;
  read(addr, std::as_writable_bytes(std::span{&val, 1}));
  return val;
}

template <size_t size>
template <typename T>
void SimpleMem<size>::write(uint64_t addr, T val) {
  write(addr, std::as_bytes(std::span{&val, 1}));
}

#endif