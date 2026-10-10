#include "machine.h"
#include "decoder.h"
#include "instructions.h"
#include "traps.h"
using decoder = lut_decoder::Decoder;

void MachineState::simulate() {
  while (!ctx.should_exit) {
    csrs.mcycle++;
    step();
    [[unlikely]]
    if (pending_trap.trap != rv_traps::Traps::None) {
      csrs.mtval = pending_trap.mtval;
      csrs.mepc = PC;
      csrs.mcause = static_cast<reg_t>(pending_trap.trap);
      PC = csrs.mtvec;
      pending_trap = {rv_traps::Traps::None, 0};
    }
  }
}

void MachineState::step() {

  // Fetch
  uint32_t instr_word{};
  ctx.fetch_count++;
  [[unlikely]]
  if (PC % (IALIGN / 8)) {
    trap(rv_traps::Traps::Instruction_address_misaligned);
    return;
  }

  auto status = mem.read(PC, instr_word);
  [[unlikely]]
  if (status != status::Ok) {
    trap(rv_traps::Traps::Instruction_access_fault);
    return;
  }

  // Decode
  auto idx = decoder::decode(instr_word);
  [[unlikely]]
  if (idx == Opcode::ILLEGAL) {
    trap(rv_traps::Traps::Illegal_instruction);
    return;
  }

  //  Execute
  instruction_functions[static_cast<size_t>(idx)](*this, instr_word);
  registers[0] = 0; // Force x0 to 0
  ctx.instr_count++;
  csrs.minstret++;
}
