#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "machine.h"
#include "traps.h"
#include "util.h"
#include <array>
#include <cstdint>
#include <exception>
#include <type_traits>
#include <unordered_map>
// TODO: set backing value to a byte to see perf impact
enum class Opcode {
  LUI,
  AUIPC,
  JAL,
  JALR,
  BEQ,
  BNE,
  BLT,
  BGE,
  BLTU,
  BGEU,
  LB,
  LH,
  LW,
  LBU,
  LHU,
  SB,
  SH,
  SW,
  ADDI,
  SLTI,
  SLTIU,
  XORI,
  ORI,
  ANDI,
  SLLI,
  SRLI,
  SRAI,
  ADD,
  SUB,
  SLL,
  SLT,
  SLTU,
  XOR,
  SRL,
  SRA,
  OR,
  AND,
  FENCE,
  ECALL,
  EBREAK,
  CSRRW,
  CSRRS,
  CSRRC,
  CSRRWI,
  CSRRSI,
  CSRRCI,
  WFI,
  COUNT,
  ILLEGAL,
  SYSTEM_PRIV,
};

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

using instr_call_t = void (*)(MachineState &, const uint32_t instr);
const std::unordered_map<Opcode, instr_call_t> instruction_functions = []() {
  return std::unordered_map<Opcode, instr_call_t>{
      {
          Opcode::LUI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) = u_imm(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::AUIPC,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) = u_imm(instr) + state.PC;
            state.PC += 4;
          },
      },
      {
          Opcode::JAL,
          [](MachineState &state, const uint32_t instr) -> void {
            reg_t target_addr = sext<20>(j_imm(instr)) + state.PC;
            if (target_addr % (IALIGN / 8))
              throw rv_traps::Traps::Instruction_address_misaligned;
            state.registers.at(rd(instr)) = state.PC += 4;
            state.PC = target_addr;
          },
      },
      {
          Opcode::JALR,
          [](MachineState &state, const uint32_t instr) -> void {
            reg_t target_addr =
                (sext<12>(i_imm(instr)) + state.registers.at(rs1(instr))) & ~1u;
            if (target_addr % (IALIGN / 8))
              throw rv_traps::Traps::Instruction_address_misaligned;
            state.registers.at(rd(instr)) = state.PC + 4;
            state.PC = target_addr;
          },
      },
      {
          Opcode::BEQ,
          [](MachineState &state, const uint32_t instr) -> void {
            const auto _rs1 = state.registers.at(rs1(instr));
            const auto _rs2 = state.registers.at(rs2(instr));
            if (_rs1 == _rs2) {
              const auto target_addr = state.PC + b_imm(instr);
              if (target_addr % (IALIGN / 8))
                throw rv_traps::Traps::Instruction_address_misaligned;
              state.PC = target_addr;
            } else
              state.PC += 4;
          },
      },
      {
          Opcode::BNE,
          [](MachineState &state, const uint32_t instr) -> void {
            const auto _rs1 = state.registers.at(rs1(instr));
            const auto _rs2 = state.registers.at(rs2(instr));
            if (_rs1 != _rs2) {
              const auto target_addr = state.PC + b_imm(instr);
              if (target_addr % (IALIGN / 8))
                throw rv_traps::Traps::Instruction_address_misaligned;
              state.PC = target_addr;
            } else
              state.PC += 4;
          },
      },
      {
          Opcode::BLT,
          [](MachineState &state, const uint32_t instr) -> void {
            const auto _rs1 = state.registers.at(rs1(instr));
            const auto _rs2 = state.registers.at(rs2(instr));
            if (std::make_signed_t<reg_t>(_rs1) <
                std::make_signed_t<reg_t>(_rs2)) {
              const auto target_addr = state.PC + b_imm(instr);
              if (target_addr % (IALIGN / 8))
                throw rv_traps::Traps::Instruction_address_misaligned;
              state.PC = target_addr;
            } else
              state.PC += 4;
          },
      },
      {
          Opcode::BGE,
          [](MachineState &state, const uint32_t instr) -> void {
            const auto _rs1 = state.registers.at(rs1(instr));
            const auto _rs2 = state.registers.at(rs2(instr));
            if (std::make_signed_t<reg_t>(_rs1) >=
                std::make_signed_t<reg_t>(_rs2)) {
              const auto target_addr = state.PC + b_imm(instr);
              if (target_addr % (IALIGN / 8))
                throw rv_traps::Traps::Instruction_address_misaligned;
              state.PC = target_addr;
            } else
              state.PC += 4;
          },
      },
      {
          Opcode::BLTU,
          [](MachineState &state, const uint32_t instr) -> void {
            const auto _rs1 = state.registers.at(rs1(instr));
            const auto _rs2 = state.registers.at(rs2(instr));
            if (_rs1 < _rs2) {
              const auto target_addr = state.PC + b_imm(instr);
              if (target_addr % (IALIGN / 8))
                throw rv_traps::Traps::Instruction_address_misaligned;
              state.PC = target_addr;
            } else
              state.PC += 4;
          },
      },
      {
          Opcode::BGEU,
          [](MachineState &state, const uint32_t instr) -> void {
            const auto _rs1 = state.registers.at(rs1(instr));
            const auto _rs2 = state.registers.at(rs2(instr));
            if (_rs1 >= _rs2) {
              const auto target_addr = state.PC + b_imm(instr);
              if (target_addr % (IALIGN / 8))
                throw rv_traps::Traps::Instruction_address_misaligned;
              state.PC = target_addr;
            } else
              state.PC += 4;
          },
      },
      {
          Opcode::LB,
          [](MachineState &state, const uint32_t instr) -> void {
            auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
            state.registers.at(rd(instr)) =
                sext<reg_t>((state.mem.read<uint8_t>(load_addr)));
            state.PC += 4;
          },
      },
      {
          Opcode::LH,
          [](MachineState &state, const uint32_t instr) -> void {
            auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
            if (load_addr % 2)
              throw rv_traps::Traps::Load_address_misaligned;
            state.registers.at(rd(instr)) =
                sext<reg_t>((state.mem.read<uint16_t>(load_addr)));
            state.PC += 4;
          },
      },
      {
          Opcode::LW,
          [](MachineState &state, const uint32_t instr) -> void {
            auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
            if (load_addr % 4)
              throw rv_traps::Traps::Load_address_misaligned;
            state.registers.at(rd(instr)) =
                sext<reg_t>((state.mem.read<uint32_t>(load_addr)));
            state.PC += 4;
          },
      },
      {
          Opcode::LBU,
          [](MachineState &state, const uint32_t instr) -> void {
            auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
            state.registers.at(rd(instr)) = state.mem.read<uint8_t>(load_addr);
            state.PC += 4;
          },
      },
      {
          Opcode::LHU,
          [](MachineState &state, const uint32_t instr) -> void {
            auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
            if (load_addr % 2)
              throw rv_traps::Traps::Load_address_misaligned;
            state.registers.at(rd(instr)) = state.mem.read<uint16_t>(load_addr);
            state.PC += 4;
          },
      },
      {
          Opcode::SB,
          [](MachineState &state, const uint32_t instr) -> void {
            auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
            state.mem.write<uint8_t>(
                store_addr,
                static_cast<uint8_t>(state.registers.at(rs2(instr))));
            state.PC += 4;
          },
      },
      {
          Opcode::SH,
          [](MachineState &state, const uint32_t instr) -> void {
            auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
            if (store_addr % 2)
              throw rv_traps::Traps::Store_AMO_address_misaligned;
            state.mem.write<uint16_t>(
                store_addr,
                static_cast<uint16_t>(state.registers.at(rs2(instr))));
            state.PC += 4;
          },
      },
      {
          Opcode::SW,
          [](MachineState &state, const uint32_t instr) -> void {
            auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
            if (store_addr % 4)
              throw rv_traps::Traps::Store_AMO_address_misaligned;
            state.mem.write<uint32_t>(
                store_addr,
                static_cast<uint32_t>(state.registers.at(rs2(instr))));
            state.PC += 4;
          },
      },
      {
          Opcode::ADDI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                state.registers.at(rs1(instr)) + i_imm(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::SLTI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                std::make_signed_t<reg_t>(state.registers.at(rs1(instr))) <
                std::make_signed_t<reg_t>(i_imm(instr));
            state.PC += 4;
          },
      },
      {
          Opcode::SLTIU,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                state.registers.at(rs1(instr)) < i_imm(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::XORI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                state.registers.at(rs1(instr)) ^ i_imm(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::ORI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                state.registers.at(rs1(instr)) | i_imm(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::ANDI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                state.registers.at(rs1(instr)) & i_imm(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::SLLI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) = state.registers.at(rs1(instr))
                                            << shamt(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::SRLI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) =
                state.registers.at(rs1(instr)) >> shamt(instr);
            state.PC += 4;
          },
      },
      {
          Opcode::SRAI,
          [](MachineState &state, const uint32_t instr) -> void {
            state.registers.at(rd(instr)) = static_cast<reg_t>(
                std::make_signed_t<reg_t>(state.registers.at(rs1(instr))) >>
                shamt(instr));
            state.PC += 4;
          },
      },
      {
          Opcode::ADD,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 + _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::SUB,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 - _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::SLL,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 << _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::SLT,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 =
                std::make_signed_t<reg_t>(state.registers.at(rs1(instr)));
            auto _rs2 =
                std::make_signed_t<reg_t>(state.registers.at(rs2(instr)));
            _rd = _rs1 < _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::SLTU,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 < _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::XOR,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 ^ _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::SRL,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 >> _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::SRA,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 =
                std::make_signed_t<reg_t>(state.registers.at(rs1(instr)));
            auto _rs2 =
                std::make_signed_t<reg_t>(state.registers.at(rs2(instr)));
            _rd = static_cast<reg_t>(_rs1 >> _rs2);
            state.PC += 4;
          },
      },
      {
          Opcode::OR,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 | _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::AND,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _rs2 = state.registers.at(rs2(instr));
            _rd = _rs1 & _rs2;
            state.PC += 4;
          },
      },
      {
          Opcode::FENCE,
          [](MachineState &state, const uint32_t) -> void { state.PC += 4; },
      },
      {
          Opcode::ECALL,
          [](MachineState &state, const uint32_t) -> void {
            throw rv_traps::Traps::Environment_call_from_M_mode;
            state.PC += 4;
          },
      },
      {
          Opcode::EBREAK,
          [](MachineState &state, const uint32_t) -> void {
            throw rv_traps::Traps::Breakpoint;
            state.PC += 4;
          },
      },
      {
          Opcode::CSRRW,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _csr = csr(instr);
            if (rd(instr))
              state.csrs.read(_csr, _rd);
            state.csrs.write(_csr, _rs1);
            state.PC += 4;
          },
      },
      {
          Opcode::CSRRS,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _csr = csr(instr);
            state.csrs.read(_csr, _rd);
            if (rs1(instr))
              state.csrs.write(_csr, _rd | _rs1);
            state.PC += 4;
          },
      },
      {
          Opcode::CSRRC,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto _rs1 = state.registers.at(rs1(instr));
            auto _csr = csr(instr);
            state.csrs.read(_csr, _rd);
            if (rs1(instr))
              state.csrs.write(_csr, _rd & ~_rs1);
            state.PC += 4;
          },
      },
      {
          Opcode::CSRRWI,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto uimm = u_imm_4(instr);
            auto _csr = csr(instr);
            if (rd(instr))
              state.csrs.read(_csr, _rd);
            state.csrs.write(_csr, uimm);
            state.PC += 4;
          },
      },
      {
          Opcode::CSRRSI,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto uimm = u_imm_4(instr);
            auto _csr = csr(instr);
            state.csrs.read(_csr, _rd);
            if (uimm)
              state.csrs.write(_csr, _rd | uimm);
            state.PC += 4;
          },
      },
      {
          Opcode::CSRRCI,
          [](MachineState &state, const uint32_t instr) -> void {
            auto &_rd = state.registers.at(rd(instr));
            auto uimm = u_imm_4(instr);
            auto _csr = csr(instr);
            state.csrs.read(_csr, _rd);
            if (uimm)
              state.csrs.write(_csr, _rd & ~uimm);
            state.PC += 4;
          },
      },
      {Opcode::WFI,
       [](MachineState &, const uint32_t) -> void { terminate = true; }},
      {Opcode::ILLEGAL,
       [](MachineState &, const uint32_t) -> void { std::terminate(); }}};
}();

#endif