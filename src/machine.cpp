#include "machine.h"
#include "decoder.h"
using decoder = lut_decoder::Decoder;

void MachineState::simulate() {
  while (!ctx.should_exit) {
    try {
      // Fetch
      // TODO: wrap fetch so Instruction_access_fault can be thrown on error
      uint32_t instr_word = mem.read<uint32_t>(PC);
      // Decode
      auto idx = decoder::decode(instr_word);
      //  Execute
      instruction_functions[static_cast<size_t>(idx)](*this, instr_word);
      registers[0] = 0; // Force x0 to 0
      ctx.instr_count++;
      csrs.mcycle++;
      csrs.minstret++;
    } catch (rv_traps::Traps &trap) {
      std::println("Terminating due to trap: {}", rv_traps::get_trap_str(trap));
      ctx.exit_code = 1;
      return;
    }
  }
}