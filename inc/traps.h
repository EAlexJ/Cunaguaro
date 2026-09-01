#ifndef TRAPS_H
#define TRAPS_H
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
namespace rv_traps {
enum class Traps : size_t {
  Instruction_address_misaligned = 0,
  Instruction_access_fault = 1,
  Illegal_instruction = 2,
  Breakpoint = 3,
  Load_address_misaligned = 4,
  Load_access_fault = 5,
  Store_AMO_address_misaligned = 6,
  Store_AMO_access_fault = 7,
  Environment_call_from_M_mode = 11,
};
constexpr std::array<std::string_view, 12> trap_strings{
    {"Instruction_address_misaligned", "Instruction_access_fault",
     "Illegal_instruction", "Breakpoint", "Load_address_misaligned",
     "Load_access_fault", "Store_AMO_address_misaligned",
     "Store_AMO_access_fault", "Not implemented", "Not implemented",
     "Not implemented", "Environment_call_from_M_mode"}};
constexpr auto get_trap_str(Traps trap) {
  return trap_strings.at(static_cast<std::underlying_type_t<Traps>>(trap));
}
} // namespace rv_traps
#endif