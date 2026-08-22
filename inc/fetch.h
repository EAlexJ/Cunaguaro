#ifndef FETCH_H
#define FETCH_H
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <type_traits>
#include <unistd.h>
template <typename T>
  requires(std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>)
class binary_loader {
  // TODO: hide the fact that this is templated from using classes, user should
  // not call if it is 32 or 64 bit, can this be hidden?
  T offset;
  size_t size;
  int fd;
  T *ptr;

  // TODO: does a singleton make sense here?
public:
  binary_loader(std::filesystem::path path, T start_offset = 0)
      : offset(start_offset) {
    fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
      throw std::runtime_error("Could not open path");

    struct stat my_stat{};
    fstat(fd, &my_stat);
    size = static_cast<const size_t>(my_stat.st_size);
    ptr = static_cast<T *>(
        mmap(0, size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0));
  }
  binary_loader(binary_loader &) = delete;
  ~binary_loader() {
    munmap(ptr, size);
    close(fd);
  }
  // TODO: overload* operator
  T at(unsigned off) { return *(ptr - offset + off); };
};
#endif