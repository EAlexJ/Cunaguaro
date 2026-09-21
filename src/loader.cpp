#include "loader.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

MemoryMappedFile::MemoryMappedFile(std::filesystem::path path) {
  if (!std::filesystem::exists(path))
    throw std::runtime_error("File does not exist");
  int fd;
  fd = open(path.c_str(), O_RDONLY);
  if (fd == -1)
    throw std::runtime_error("Could not open path");

  struct stat my_stat{};
  fstat(fd, &my_stat);
  size = static_cast<size_t>(my_stat.st_size);
  ptr = mmap(0, size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED)
    throw std::runtime_error("Map failed for some reason");
  close(fd);
}

MemoryMappedFile::~MemoryMappedFile() { munmap(ptr, size); }
