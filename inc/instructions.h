#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "general.h"
#include "machine.h"
#include "opcode.h"
#include "traps.h"
#include "util.h"
#include <cstdint>

constexpr auto rd(uint32_t instr) { return bit_range<11, 7>(instr); }
constexpr auto rs1(uint32_t instr) { return bit_range<19, 15>(instr); }
constexpr auto rs2(uint32_t instr) { return bit_range<24, 20>(instr); }
constexpr auto i_imm(uint32_t instr) {
  return sext<11>(bit_range<31, 20>(instr));
};
constexpr auto s_imm(uint32_t instr) {
  return sext<11>(bit_range<31, 25>(instr) << 5) |
         bit_range<11, 8>(instr) << 1 | bit_range<7, 7>(instr);
};
constexpr auto b_imm(uint32_t instr) {
  return sext<12>(bit_range<31, 31>(instr) << 12) |
         bit_range<7, 7>(instr) << 11 | bit_range<30, 25>(instr) << 5 |
         bit_range<11, 8>(instr) << 1;
};
constexpr auto u_imm(uint32_t instr) { return bit_range<31, 12>(instr) << 12; };
constexpr auto j_imm(uint32_t instr) {
  return sext<20>(bit_range<31, 31>(instr) << 20) |
         bit_range<19, 12>(instr) << 12 | bit_range<20, 20>(instr) << 11 |
         bit_range<30, 21>(instr) << 1;
};
constexpr auto shamt(uint32_t instr) { return bit_range<24, 20>(instr); }
constexpr auto u_imm_4(uint32_t instr) { return rs1(instr); }
constexpr auto csr(uint32_t instr) { return bit_range<31, 20>(instr); }

[[gnu::always_inline]]
constexpr void execute_instruction(Opcode op, MachineState &state,
                                   const uint32_t instr) noexcept {
  switch (op) {
  case Opcode::LUI: {
    state.registers[rd(instr)] = u_imm(instr);
    state.PC += 4;
    break;
  }

  case Opcode::AUIPC: {
    state.registers[rd(instr)] = u_imm(instr) + state.PC;
    state.PC += 4;
    break;
  }

  case Opcode::JAL: {
    reg_t target_addr = sext<20>(j_imm(instr)) + state.PC;
    if (target_addr % (IALIGN / 8)) {
      state.trap(rv_traps::Traps::Instruction_address_misaligned);
      return;
    }
    state.registers[rd(instr)] = state.PC += 4;
    state.PC = target_addr;
    break;
  }

  case Opcode::JALR: {
    reg_t target_addr =
        (sext<12>(i_imm(instr)) + state.registers[rs1(instr)]) & ~1u;
    if (target_addr % (IALIGN / 8)) {
      state.trap(rv_traps::Traps::Instruction_address_misaligned);
      return;
    }
    state.registers[rd(instr)] = state.PC + 4;
    state.PC = target_addr;
    break;
  }

  case Opcode::BEQ: {
    const auto _rs1 = state.registers[rs1(instr)];
    const auto _rs2 = state.registers[rs2(instr)];
    if (_rs1 == _rs2) {
      const auto target_addr = state.PC + b_imm(instr);
      if (target_addr % (IALIGN / 8)) {
        state.trap(rv_traps::Traps::Instruction_address_misaligned);
        return;
      }
      state.PC = target_addr;
    } else {
      state.PC += 4;
    }
    break;
  }

  case Opcode::BNE: {
    const auto _rs1 = state.registers[rs1(instr)];
    const auto _rs2 = state.registers[rs2(instr)];
    if (_rs1 != _rs2) {
      const auto target_addr = state.PC + b_imm(instr);
      if (target_addr % (IALIGN / 8)) {
        state.trap(rv_traps::Traps::Instruction_address_misaligned);
        return;
      }
      state.PC = target_addr;
    } else {
      state.PC += 4;
    }
    break;
  }

  case Opcode::BLT: {
    const auto _rs1 = state.registers[rs1(instr)];
    const auto _rs2 = state.registers[rs2(instr)];
    if (std::make_signed_t<reg_t>(_rs1) < std::make_signed_t<reg_t>(_rs2)) {
      const auto target_addr = state.PC + b_imm(instr);
      if (target_addr % (IALIGN / 8)) {
        state.trap(rv_traps::Traps::Instruction_address_misaligned);
        return;
      }
      state.PC = target_addr;
    } else {
      state.PC += 4;
    }
    break;
  }

  case Opcode::BGE: {
    const auto _rs1 = state.registers[rs1(instr)];
    const auto _rs2 = state.registers[rs2(instr)];
    if (std::make_signed_t<reg_t>(_rs1) >= std::make_signed_t<reg_t>(_rs2)) {
      const auto target_addr = state.PC + b_imm(instr);
      if (target_addr % (IALIGN / 8)) {
        state.trap(rv_traps::Traps::Instruction_address_misaligned);
        return;
      }
      state.PC = target_addr;
    } else {
      state.PC += 4;
    }
    break;
  }

  case Opcode::BLTU: {
    const auto _rs1 = state.registers[rs1(instr)];
    const auto _rs2 = state.registers[rs2(instr)];
    if (_rs1 < _rs2) {
      const auto target_addr = state.PC + b_imm(instr);
      if (target_addr % (IALIGN / 8)) {
        state.trap(rv_traps::Traps::Instruction_address_misaligned);
        return;
      }
      state.PC = target_addr;
    } else {
      state.PC += 4;
    }
    break;
  }

  case Opcode::BGEU: {
    const auto _rs1 = state.registers[rs1(instr)];
    const auto _rs2 = state.registers[rs2(instr)];
    if (_rs1 >= _rs2) {
      const auto target_addr = state.PC + b_imm(instr);
      if (target_addr % (IALIGN / 8)) {
        state.trap(rv_traps::Traps::Instruction_address_misaligned);
        return;
      }
      state.PC = target_addr;
    } else {
      state.PC += 4;
    }
    break;
  }

  case Opcode::LB: {
    auto load_addr = state.registers[rs1(instr)] + i_imm(instr);
    auto &_rd = state.registers[rd(instr)];
    uint8_t read_val{};
    auto status = state.mem.read(load_addr, read_val);
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Load_access_fault);
      return;
    }
    _rd = sext<reg_t>(read_val);
    state.PC += 4;
    break;
  }

  case Opcode::LH: {
    auto load_addr = state.registers[rs1(instr)] + i_imm(instr);
    [[unlikely]]
    if (load_addr % 2) {
      state.trap(rv_traps::Traps::Load_address_misaligned);
      return;
    }
    auto &_rd = state.registers[rd(instr)];
    uint16_t read_val{};
    auto status = state.mem.read(load_addr, read_val);
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Load_access_fault);
      return;
    }
    _rd = sext<reg_t>(read_val);
    state.PC += 4;
    break;
  }

  case Opcode::LW: {
    auto load_addr = state.registers[rs1(instr)] + i_imm(instr);
    [[unlikely]]
    if (load_addr % 4) {
      state.trap(rv_traps::Traps::Load_address_misaligned);
      return;
    }
    auto &_rd = state.registers[rd(instr)];
    uint32_t read_val{};
    auto status = state.mem.read(load_addr, read_val);
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Load_access_fault);
      return;
    }
    _rd = sext<reg_t>(read_val);
    state.PC += 4;
    break;
  }

  case Opcode::LBU: {
    auto load_addr = state.registers[rs1(instr)] + i_imm(instr);
    auto &_rd = state.registers[rd(instr)];
    uint8_t read_val{};
    auto status = state.mem.read(load_addr, read_val);
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Load_access_fault);
      return;
    }
    _rd = read_val;
    state.PC += 4;
    break;
  }

  case Opcode::LHU: {
    auto load_addr = state.registers[rs1(instr)] + i_imm(instr);
    [[unlikely]]
    if (load_addr % 2) {
      state.trap(rv_traps::Traps::Load_address_misaligned);
      return;
    }
    auto &_rd = state.registers[rd(instr)];
    uint16_t read_val{};
    auto status = state.mem.read(load_addr, read_val);
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Load_access_fault);
      return;
    }
    _rd = read_val;
    state.PC += 4;
    break;
  }

  case Opcode::SB: {
    auto store_addr = state.registers[rs1(instr)] + s_imm(instr);
    auto status = state.mem.write(
        store_addr, static_cast<uint8_t>(state.registers[rs2(instr)]));
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Store_AMO_access_fault);
      return;
    }
    state.PC += 4;
    break;
  }

  case Opcode::SH: {
    auto store_addr = state.registers[rs1(instr)] + s_imm(instr);
    [[unlikely]] if (store_addr % 2) {
      state.trap(rv_traps::Traps::Store_AMO_address_misaligned);
      return;
    }
    auto status = state.mem.write(
        store_addr, static_cast<uint16_t>(state.registers[rs2(instr)]));
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Store_AMO_access_fault);
      return;
    }
    state.PC += 4;
    break;
  }

  case Opcode::SW: {
    auto store_addr = state.registers[rs1(instr)] + s_imm(instr);
    [[unlikely]] if (store_addr % 4) {
      state.trap(rv_traps::Traps::Store_AMO_address_misaligned);
      return;
    }
    auto status = state.mem.write(
        store_addr, static_cast<uint32_t>(state.registers[rs2(instr)]));
    [[unlikely]]
    if (status != status::Ok) {
      state.trap(rv_traps::Traps::Store_AMO_access_fault);
      return;
    }
    state.PC += 4;
    break;
  }

  case Opcode::ADDI: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] + i_imm(instr);
    state.PC += 4;
    break;
  }

  case Opcode::SLTI: {
    state.registers[rd(instr)] =
        std::make_signed_t<reg_t>(state.registers[rs1(instr)]) <
        std::make_signed_t<reg_t>(i_imm(instr));
    state.PC += 4;
    break;
  }

  case Opcode::SLTIU: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] < i_imm(instr);
    state.PC += 4;
    break;
  }

  case Opcode::XORI: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] ^ i_imm(instr);
    state.PC += 4;
    break;
  }

  case Opcode::ORI: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] | i_imm(instr);
    state.PC += 4;
    break;
  }

  case Opcode::ANDI: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] & i_imm(instr);
    state.PC += 4;
    break;
  }

  case Opcode::SLLI: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] << shamt(instr);
    state.PC += 4;
    break;
  }

  case Opcode::SRLI: {
    state.registers[rd(instr)] = state.registers[rs1(instr)] >> shamt(instr);
    state.PC += 4;
    break;
  }

  case Opcode::SRAI: {
    state.registers[rd(instr)] = static_cast<reg_t>(
        std::make_signed_t<reg_t>(state.registers[rs1(instr)]) >> shamt(instr));
    state.PC += 4;
    break;
  }

  case Opcode::ADD: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 + _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::SUB: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 - _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::SLL: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 << _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::SLT: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = std::make_signed_t<reg_t>(state.registers[rs1(instr)]);
    auto _rs2 = std::make_signed_t<reg_t>(state.registers[rs2(instr)]);
    _rd = _rs1 < _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::SLTU: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 < _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::XOR: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 ^ _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::SRL: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 >> _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::SRA: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = std::make_signed_t<reg_t>(state.registers[rs1(instr)]);
    auto _rs2 = std::make_signed_t<reg_t>(state.registers[rs2(instr)]);
    _rd = static_cast<reg_t>(_rs1 >> _rs2);
    state.PC += 4;
    break;
  }

  case Opcode::OR: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 | _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::AND: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _rs2 = state.registers[rs2(instr)];
    _rd = _rs1 & _rs2;
    state.PC += 4;
    break;
  }

  case Opcode::FENCE: {
    state.PC += 4;
    break;
  }

  case Opcode::ECALL: {
    state.trap(rv_traps::Traps::Environment_call_from_M_mode);
    break;
  }

  case Opcode::EBREAK: {
    state.trap(rv_traps::Traps::Breakpoint);
    break;
  }

  case Opcode::CSRRW: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _csr = csr(instr);
    reg_t maybe_rd{};
    auto status = state.csrs.read(_csr, maybe_rd);
    [[unlikely]] if (status != status::Ok) {
      state.trap(rv_traps::Traps::Illegal_instruction);
      return;
    }
    if (rd(instr)) {
      _rd = maybe_rd;
    }
    auto _ = state.csrs.write(_csr, _rs1);
    state.PC += 4;
    break;
  }

  case Opcode::CSRRS: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _csr = csr(instr);

    auto status = state.csrs.read(_csr, _rd);
    [[unlikely]] if (status != status::Ok) {
      state.trap(rv_traps::Traps::Illegal_instruction);
      return;
    }
    if (rs1(instr)) {
      auto _ = state.csrs.write(_csr, _rd | _rs1);
    }
    state.PC += 4;
    break;
  }

  case Opcode::CSRRC: {
    auto &_rd = state.registers[rd(instr)];
    auto _rs1 = state.registers[rs1(instr)];
    auto _csr = csr(instr);
    auto status = state.csrs.read(_csr, _rd);
    [[unlikely]] if (status != status::Ok) {
      state.trap(rv_traps::Traps::Illegal_instruction);
      return;
    }
    if (rs1(instr)) {
      auto _ = state.csrs.write(_csr, _rd & ~_rs1);
    }
    state.PC += 4;
    break;
  }

  case Opcode::CSRRWI: {
    auto &_rd = state.registers[rd(instr)];
    auto uimm = u_imm_4(instr);
    auto _csr = csr(instr);
    reg_t maybe_rd{};
    auto status = state.csrs.read(_csr, maybe_rd);
    [[unlikely]] if (status != status::Ok) {
      state.trap(rv_traps::Traps::Illegal_instruction);
      return;
    }
    if (rd(instr)) {
      _rd = maybe_rd;
    }
    auto _ = state.csrs.write(_csr, uimm);
    state.PC += 4;
    break;
  }

  case Opcode::CSRRSI: {
    auto &_rd = state.registers[rd(instr)];
    auto uimm = u_imm_4(instr);
    auto _csr = csr(instr);
    auto status = state.csrs.read(_csr, _rd);
    [[unlikely]] if (status != status::Ok) {
      state.trap(rv_traps::Traps::Illegal_instruction);
      return;
    }
    if (uimm) {
      auto _ = state.csrs.write(_csr, _rd | uimm);
    }
    state.PC += 4;
    break;
  }

  case Opcode::CSRRCI: {
    auto &_rd = state.registers[rd(instr)];
    auto uimm = u_imm_4(instr);
    auto _csr = csr(instr);
    auto status = state.csrs.read(_csr, _rd);
    [[unlikely]] if (status != status::Ok) {
      state.trap(rv_traps::Traps::Illegal_instruction);
      return;
    }
    if (uimm) {
      auto _ = state.csrs.write(_csr, _rd & ~uimm);
    }
    state.PC += 4;
    break;
  }

  case Opcode::WFI: {
    state.request_stop(1);
    break;
  }

  default: {
    state.trap(rv_traps::Traps::Illegal_instruction);
    break;
  }
  }
}
#endif