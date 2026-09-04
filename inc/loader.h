#ifndef LOADER_H
#define LOADER_H
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <span>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <type_traits>
#include <unistd.h>

struct BinaryView {
  std::span<const std::byte> view;
  uint64_t _base_offset;
  BinaryView() {};
  BinaryView(void *ptr, size_t size, uint64_t base_offset)
      : view(static_cast<const std::byte *>(ptr), size),
        _base_offset(base_offset) {}

  template <typename T>
    requires std::is_trivial_v<T>
  [[nodiscard]]
  T at(uint64_t addr) {
    auto file_offset = addr - _base_offset;
    if (addr < _base_offset || file_offset + sizeof(T) > view.size())
      throw std::out_of_range("");
    T val;
    std::memcpy(&val, view.data() + file_offset, sizeof(T));
    return val;
  }
};

struct MemoryMappedFile {
  size_t size;
  int fd;
  void *ptr;
  MemoryMappedFile(std::filesystem::path path) {
    if (!std::filesystem::exists(path))
      throw std::runtime_error("File does not exist");
    fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
      throw std::runtime_error("Could not open path");

    struct stat my_stat{};
    fstat(fd, &my_stat);
    size = static_cast<const size_t>(my_stat.st_size);
    ptr = mmap(0, size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
  }
  MemoryMappedFile(const MemoryMappedFile &) = delete;
  MemoryMappedFile(const MemoryMappedFile &&) = delete;
  ~MemoryMappedFile() {
    munmap(ptr, size);
    close(fd);
  }
};

class BinaryLoader {
  MemoryMappedFile file;
  BinaryView view;

public:
  BinaryLoader(std::filesystem::path path, unsigned start_offset)
      : file(path), view(file.ptr, file.size, start_offset) {}

  template <typename T>
    requires std::is_trivially_destructible_v<T>
  [[nodiscard]]
  T at(uint64_t addr) {
    return view.at<T>(addr);
  };
};

#endif