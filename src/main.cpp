#include "fetch.h"
#include <cstdint>
#include <filesystem>

int main() {
  std::filesystem::path path{"/Users/alex/Code/ISS/build/Debug/iss-sim"};
  auto loaded_bin = binary_loader(path);
  [[maybe_unused]]
  auto elem = loaded_bin.at<uint32_t>(0);
  return 0;
}