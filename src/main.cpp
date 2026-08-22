#include "fetch.h"
#include <filesystem>

int main() {
  std::filesystem::path path{"/Users/alex/Code/ISS/build/Debug/iss-sim"};
  auto loaded_bin = binary_loader<uint32_t>(path);
  return 0;
}