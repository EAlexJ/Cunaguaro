#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "machine.h"
#include "traps.h"
#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <type_traits>
struct InstructionDescription {
  const uint32_t value;
  const uint32_t mask;
  const std::string name;
};
constexpr unsigned instruction_count = 47;
constexpr unsigned IALIGN = 32;
constexpr std::array<const InstructionDescription, instruction_count>
    instructions{{
        {0b00000000000000000000000000110111, 0b00000000000000000000000001111111,
         "LUI"},
        {0b00000000000000000000000000010111, 0b00000000000000000000000001111111,
         "AUIPC"},
        {0b00000000000000000000000001101111, 0b00000000000000000000000001111111,
         "JAL"},
        {0b00000000000000000000000001100111, 0b00000000000000000111000001111111,
         "JALR"},
        {0b00000000000000000000000001100011, 0b00000000000000000111000001111111,
         "BEQ"},
        {0b00000000000000000001000001100011, 0b00000000000000000111000001111111,
         "BNE"},
        {0b00000000000000000100000001100011, 0b00000000000000000111000001111111,
         "BLT"},
        {0b00000000000000000101000001100011, 0b00000000000000000111000001111111,
         "BGE"},
        {0b00000000000000000110000001100011, 0b00000000000000000111000001111111,
         "BLTU"},
        {0b00000000000000000111000001100011, 0b00000000000000000111000001111111,
         "BGEU"},
        {0b00000000000000000000000000000011, 0b00000000000000000111000001111111,
         "LB"},
        {0b00000000000000000001000000000011, 0b00000000000000000111000001111111,
         "LH"},
        {0b00000000000000000010000000000011, 0b00000000000000000111000001111111,
         "LW"},
        {0b00000000000000000100000000000011, 0b00000000000000000111000001111111,
         "LBU"},
        {0b00000000000000000101000000000011, 0b00000000000000000111000001111111,
         "LHU"},
        {0b00000000000000000000000000100011, 0b00000000000000000111000001111111,
         "SB"},
        {0b00000000000000000001000000100011, 0b00000000000000000111000001111111,
         "SH"},
        {0b00000000000000000010000000100011, 0b00000000000000000111000001111111,
         "SW"},
        {0b00000000000000000000000000010011, 0b00000000000000000111000001111111,
         "ADDI"},
        {0b00000000000000000010000000010011, 0b00000000000000000111000001111111,
         "SLTI"},
        {0b00000000000000000011000000010011, 0b00000000000000000111000001111111,
         "SLTIU"},
        {0b00000000000000000100000000010011, 0b00000000000000000111000001111111,
         "XORI"},
        {0b00000000000000000110000000010011, 0b00000000000000000111000001111111,
         "ORI"},
        {0b00000000000000000111000000010011, 0b00000000000000000111000001111111,
         "ANDI"},
        {0b00000000000000000001000000010011, 0b11111110000000000111000001111111,
         "SLLI"},
        {0b00000000000000000101000000010011, 0b11111110000000000111000001111111,
         "SRLI"},
        {0b01000000000000000101000000010011, 0b11111110000000000111000001111111,
         "SRAI"},
        {0b00000000000000000000000000110011, 0b11111110000000000111000001111111,
         "ADD"},
        {0b01000000000000000000000000110011, 0b11111110000000000111000001111111,
         "SUB"},
        {0b00000000000000000001000000110011, 0b11111110000000000111000001111111,
         "SLL"},
        {0b00000000000000000010000000110011, 0b11111110000000000111000001111111,
         "SLT"},
        {0b00000000000000000011000000110011, 0b11111110000000000111000001111111,
         "SLTU"},
        {0b00000000000000000100000000110011, 0b11111110000000000111000001111111,
         "XOR"},
        {0b00000000000000000101000000110011, 0b11111110000000000111000001111111,
         "SRL"},
        {0b01000000000000000101000000110011, 0b11111110000000000111000001111111,
         "SRA"},
        {0b00000000000000000110000000110011, 0b11111110000000000111000001111111,
         "OR"},
        {0b00000000000000000111000000110011, 0b11111110000000000111000001111111,
         "AND"},
        {0b00000000000000000000000000001111, 0b00000000000000000111000001111111,
         "FENCE"},
        {0b00000000000000000000000001110011, 0b11111111111111111111111111111111,
         "ECALL"},
        {0b00000000000100000000000001110011, 0b11111111111111111111111111111111,
         "EBREAK"},
        {0b00000000000000000001000001110011, 0b00000000000000000111000001111111,
         "CSRRW"},
        {0b00000000000000000010000001110011, 0b00000000000000000111000001111111,
         "CSRRS"},
        {0b00000000000000000011000001110011, 0b00000000000000000111000001111111,
         "CSRRC"},
        {0b00000000000000000101000001110011, 0b00000000000000000111000001111111,
         "CSRRWI"},
        {0b00000000000000000110000001110011, 0b00000000000000000111000001111111,
         "CSRRSI"},
        {0b00000000000000000111000001110011, 0b00000000000000000111000001111111,
         "CSRRCI"},
        {0b00010000010100000000000001110011, 0b11111111111111111111111111111111,
         "WFI"},

    }};

template <uint32_t upper, uint32_t lower, typename T>
  requires std::is_unsigned_v<T>
constexpr T bit_range(T value) {
  static_assert(upper <= sizeof(T) * 8,
                "Upper is out of range for supplied type");
  static_assert(lower <= sizeof(T) * 8,
                "Lower is out of range for supplied type");
  return (value >> lower) &
         static_cast<T>(std::bitset<upper + 1 - lower>().set().to_ulong());
}
template <uint8_t bitpos, typename T> constexpr T sext(T val) {
  static_assert(bitpos <= sizeof(T) * 8,
                "Trying to sign extend beyond supplied types size");
  using signed_t = std::make_signed_t<T>;
  auto shamt = sizeof(T) * 8 - bitpos - 1;
  return static_cast<T>(static_cast<signed_t>(val) << shamt >> shamt);
}
template <typename T, typename I> T sext(I init) {
  return static_cast<T>(static_cast<std::make_signed_t<T>>(
      static_cast<std::make_signed_t<I>>(init)));
}
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

constexpr std::array<void (*)(MachineState &, const uint32_t instr),
                     instruction_count>
    instruction_functions{
        // LUI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) = u_imm(instr);
          state.PC += 4;
        },
        // AUIPC
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) = u_imm(instr) + state.PC;
          state.PC += 4;
        },
        // JAL
        [](MachineState &state, const uint32_t instr) -> void {
          reg_t target_addr = sext<20>(j_imm(instr)) + state.PC;
          if (target_addr % (IALIGN / 8))
            throw rv_traps::Traps::Instruction_address_misaligned;
          state.registers.at(rd(instr)) = state.PC += 4;
          state.PC = target_addr;
        },
        // JALR
        [](MachineState &state, const uint32_t instr) -> void {
          reg_t target_addr =
              (sext<12>(i_imm(instr)) + state.registers.at(rs1(instr))) & ~1u;
          if (target_addr % (IALIGN / 8))
            throw rv_traps::Traps::Instruction_address_misaligned;
          state.registers.at(rd(instr)) = state.PC + 4;
          state.PC = target_addr;
        },
        // BEQ
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
        // BNE
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
        // BLT
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
        // BGE
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
        // BLTU
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
        // BGEU
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
        // LB
        [](MachineState &state, const uint32_t instr) -> void {
          auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
          state.registers.at(rd(instr)) =
              sext<reg_t>((state.mem.read<uint8_t>(load_addr)));
          state.PC += 4;
        },
        // LH
        [](MachineState &state, const uint32_t instr) -> void {
          auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
          if (load_addr % 2)
            throw rv_traps::Traps::Load_address_misaligned;
          state.registers.at(rd(instr)) =
              sext<reg_t>((state.mem.read<uint16_t>(load_addr)));
          state.PC += 4;
        },
        // LW
        [](MachineState &state, const uint32_t instr) -> void {
          auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
          if (load_addr % 4)
            throw rv_traps::Traps::Load_address_misaligned;
          state.registers.at(rd(instr)) =
              sext<reg_t>((state.mem.read<uint32_t>(load_addr)));
          state.PC += 4;
        },
        // LBU
        [](MachineState &state, const uint32_t instr) -> void {
          auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
          state.registers.at(rd(instr)) = state.mem.read<uint8_t>(load_addr);
          state.PC += 4;
        },
        // LHU
        [](MachineState &state, const uint32_t instr) -> void {
          auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
          if (load_addr % 2)
            throw rv_traps::Traps::Load_address_misaligned;
          state.registers.at(rd(instr)) = state.mem.read<uint16_t>(load_addr);
          state.PC += 4;
        },
        // SB
        [](MachineState &state, const uint32_t instr) -> void {
          auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
          state.mem.write<uint8_t>(
              store_addr, static_cast<uint8_t>(state.registers.at(rs2(instr))));
          state.PC += 4;
        },
        // SH
        [](MachineState &state, const uint32_t instr) -> void {
          auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
          if (store_addr % 2)
            throw rv_traps::Traps::Store_AMO_address_misaligned;
          state.mem.write<uint16_t>(
              store_addr,
              static_cast<uint16_t>(state.registers.at(rs2(instr))));
          state.PC += 4;
        },
        // SW
        [](MachineState &state, const uint32_t instr) -> void {
          auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
          if (store_addr % 4)
            throw rv_traps::Traps::Store_AMO_address_misaligned;
          state.mem.write<uint32_t>(
              store_addr,
              static_cast<uint32_t>(state.registers.at(rs2(instr))));
          state.PC += 4;
        },
        // ADDI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              state.registers.at(rs1(instr)) + i_imm(instr);
          state.PC += 4;
        },
        // SLTI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              std::make_signed_t<reg_t>(state.registers.at(rs1(instr))) <
              std::make_signed_t<reg_t>(i_imm(instr));
          state.PC += 4;
        },
        // SLTIU
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              state.registers.at(rs1(instr)) < i_imm(instr);
          state.PC += 4;
        },
        // XORI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              state.registers.at(rs1(instr)) ^ i_imm(instr);
          state.PC += 4;
        },
        // ORI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              state.registers.at(rs1(instr)) | i_imm(instr);
          state.PC += 4;
        },
        // ANDI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              state.registers.at(rs1(instr)) & i_imm(instr);
          state.PC += 4;
        },
        // SLLI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) = state.registers.at(rs1(instr))
                                          << shamt(instr);
          state.PC += 4;
        },
        // SRLI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) =
              state.registers.at(rs1(instr)) >> shamt(instr);
          state.PC += 4;
        },
        // SRAI
        [](MachineState &state, const uint32_t instr) -> void {
          state.registers.at(rd(instr)) = static_cast<reg_t>(
              std::make_signed_t<reg_t>(state.registers.at(rs1(instr))) >>
              shamt(instr));
          state.PC += 4;
        },
        // ADD
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 + _rs2;
          state.PC += 4;
        },
        // SUB
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 - _rs2;
          state.PC += 4;
        },
        // SLL
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 << _rs2;
          state.PC += 4;
        },
        // SLT
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = std::make_signed_t<reg_t>(state.registers.at(rs1(instr)));
          auto _rs2 = std::make_signed_t<reg_t>(state.registers.at(rs2(instr)));
          _rd = _rs1 < _rs2;
          state.PC += 4;
        },
        // SLTU
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 < _rs2;
          state.PC += 4;
        },
        // XOR
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 ^ _rs2;
          state.PC += 4;
        },
        // SRL
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 >> _rs2;
          state.PC += 4;
        },
        // SRA
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = std::make_signed_t<reg_t>(state.registers.at(rs1(instr)));
          auto _rs2 = std::make_signed_t<reg_t>(state.registers.at(rs2(instr)));
          _rd = static_cast<reg_t>(_rs1 >> _rs2);
          state.PC += 4;
        },
        // OR
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 | _rs2;
          state.PC += 4;
        },
        // AND
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _rs2 = state.registers.at(rs2(instr));
          _rd = _rs1 & _rs2;
          state.PC += 4;
        },
        // FENCE
        [](MachineState &state, const uint32_t) -> void { state.PC += 4; },
        // ECALL
        [](MachineState &state, const uint32_t) -> void {
          throw rv_traps::Traps::Environment_call_from_M_mode;
          state.PC += 4;
        },
        // EBREAK
        [](MachineState &state, const uint32_t) -> void {
          throw rv_traps::Traps::Breakpoint;
          state.PC += 4;
        },
        // CSRRW
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _csr = csr(instr);
          if (rd(instr))
            state.csrs.read(_csr, _rd);
          state.csrs.write(_csr, _rs1);
          state.PC += 4;
        },
        // CSRRS
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _csr = csr(instr);
          state.csrs.read(_csr, _rd);
          if (rs1(instr))
            state.csrs.write(_csr, _rd | _rs1);
          state.PC += 4;
        },
        // CSRRC
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto _rs1 = state.registers.at(rs1(instr));
          auto _csr = csr(instr);
          state.csrs.read(_csr, _rd);
          if (rs1(instr))
            state.csrs.write(_csr, _rd & ~_rs1);
          state.PC += 4;
        },
        // CSRRWI
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto uimm = u_imm_4(instr);
          auto _csr = csr(instr);
          if (rd(instr))
            state.csrs.read(_csr, _rd);
          state.csrs.write(_csr, uimm);
          state.PC += 4;
        },
        // CSRRSI
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto uimm = u_imm_4(instr);
          auto _csr = csr(instr);
          state.csrs.read(_csr, _rd);
          if (uimm)
            state.csrs.write(_csr, _rd | uimm);
          state.PC += 4;
        },
        // CSRRCI
        [](MachineState &state, const uint32_t instr) -> void {
          auto &_rd = state.registers.at(rd(instr));
          auto uimm = u_imm_4(instr);
          auto _csr = csr(instr);
          state.csrs.read(_csr, _rd);
          if (uimm)
            state.csrs.write(_csr, _rd & ~uimm);
          state.PC += 4;
        },
        // WFI
        [](MachineState &, const uint32_t) -> void { terminate = true; },
    };
#endif