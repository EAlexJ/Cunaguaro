#ifndef FETCH_H
#define FETCH_H
#include <cstddef>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

class binary_loader {
  // TODO: hide the fact that this is templated from using classes, user should
  // not call if it is 32 or 64 bit, can this be hidden?
  unsigned byte_offset;
  size_t size;
  int fd;
  void *ptr;

  // TODO: does a singleton make sense here?
public:
  binary_loader(std::filesystem::path path, unsigned start_offset = 0)
      : byte_offset(start_offset) {
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
  // TODO: overload* operator
  template <typename T> T *get_ptr(unsigned off) {
    return reinterpret_cast<T *>(static_cast<std::byte *>(ptr) - byte_offset) +
           off;
  };
  template <typename T> T at(unsigned off) { return *get_ptr<T>(off); }
};
#endif