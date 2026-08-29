#ifndef LOADER_H
#define LOADER_H
#include <cstddef>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

class binary_loader {
  unsigned byte_offset;
  size_t size;
  int fd;
  void *ptr;

  // TODO: does a singleton make sense here?
public:
  binary_loader(std::filesystem::path path, unsigned start_offset = 0)
      : byte_offset(start_offset) {
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
  binary_loader(binary_loader &) = delete;
  ~binary_loader() {
    munmap(ptr, size);
    close(fd);
  }
  template <typename T> T *get_ptr(uint64_t off) {
    return reinterpret_cast<T *>(static_cast<std::byte *>(ptr) - byte_offset +
                                 off);
  };
  template <typename T> T at(uint64_t off) { return *get_ptr<T>(off); }
  uint32_t operator[](uint64_t off) { return *get_ptr<uint32_t>(off); }
};
#endif