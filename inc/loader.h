#ifndef LOADER_H
#define LOADER_H

#include <cstddef>
#include <filesystem>
#include <span>
struct MemoryMappedFile {
  size_t size;
  void *ptr;
  MemoryMappedFile(std::filesystem::path path);
  MemoryMappedFile(const MemoryMappedFile &) = delete;
  MemoryMappedFile(const MemoryMappedFile &&) = delete;
  ~MemoryMappedFile();
};

void load_elf_file(std::span<std::byte>, std::filesystem::path);

#endif