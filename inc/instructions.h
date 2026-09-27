#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "machine.h"
#include "traps.h"
#include <array>
#include <bitset>
#include <cstdint>
#include <exception>
#include <type_traits>
#include <unordered_map>

enum class InstructionEnum {
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
  INVALID = std::numeric_limits<std::underlying_type_t<InstructionEnum>>::max(),
};

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

using instr_call_t = void (*)(MachineState &, const uint32_t instr);
const std::unordered_map<InstructionEnum, instr_call_t> instruction_functions =
    []() {
      return std::unordered_map<InstructionEnum, instr_call_t>{
          {
              InstructionEnum::LUI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) = u_imm(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::AUIPC,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) = u_imm(instr) + state.PC;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::JAL,
              [](MachineState &state, const uint32_t instr) -> void {
                reg_t target_addr = sext<20>(j_imm(instr)) + state.PC;
                if (target_addr % (IALIGN / 8))
                  throw rv_traps::Traps::Instruction_address_misaligned;
                state.registers.at(rd(instr)) = state.PC += 4;
                state.PC = target_addr;
              },
          },
          {
              InstructionEnum::JALR,
              [](MachineState &state, const uint32_t instr) -> void {
                reg_t target_addr =
                    (sext<12>(i_imm(instr)) + state.registers.at(rs1(instr))) &
                    ~1u;
                if (target_addr % (IALIGN / 8))
                  throw rv_traps::Traps::Instruction_address_misaligned;
                state.registers.at(rd(instr)) = state.PC + 4;
                state.PC = target_addr;
              },
          },
          {
              InstructionEnum::BEQ,
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
              InstructionEnum::BNE,
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
              InstructionEnum::BLT,
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
              InstructionEnum::BGE,
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
              InstructionEnum::BLTU,
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
              InstructionEnum::BGEU,
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
              InstructionEnum::LB,
              [](MachineState &state, const uint32_t instr) -> void {
                auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
                state.registers.at(rd(instr)) =
                    sext<reg_t>((state.mem.read<uint8_t>(load_addr)));
                state.PC += 4;
              },
          },
          {
              InstructionEnum::LH,
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
              InstructionEnum::LW,
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
              InstructionEnum::LBU,
              [](MachineState &state, const uint32_t instr) -> void {
                auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
                state.registers.at(rd(instr)) =
                    state.mem.read<uint8_t>(load_addr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::LHU,
              [](MachineState &state, const uint32_t instr) -> void {
                auto load_addr = state.registers.at(rs1(instr)) + i_imm(instr);
                if (load_addr % 2)
                  throw rv_traps::Traps::Load_address_misaligned;
                state.registers.at(rd(instr)) =
                    state.mem.read<uint16_t>(load_addr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SB,
              [](MachineState &state, const uint32_t instr) -> void {
                auto store_addr = state.registers.at(rs1(instr)) + s_imm(instr);
                state.mem.write<uint8_t>(
                    store_addr,
                    static_cast<uint8_t>(state.registers.at(rs2(instr))));
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SH,
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
              InstructionEnum::SW,
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
              InstructionEnum::ADDI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    state.registers.at(rs1(instr)) + i_imm(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SLTI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    std::make_signed_t<reg_t>(state.registers.at(rs1(instr))) <
                    std::make_signed_t<reg_t>(i_imm(instr));
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SLTIU,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    state.registers.at(rs1(instr)) < i_imm(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::XORI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    state.registers.at(rs1(instr)) ^ i_imm(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::ORI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    state.registers.at(rs1(instr)) | i_imm(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::ANDI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    state.registers.at(rs1(instr)) & i_imm(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SLLI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) = state.registers.at(rs1(instr))
                                                << shamt(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SRLI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) =
                    state.registers.at(rs1(instr)) >> shamt(instr);
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SRAI,
              [](MachineState &state, const uint32_t instr) -> void {
                state.registers.at(rd(instr)) = static_cast<reg_t>(
                    std::make_signed_t<reg_t>(state.registers.at(rs1(instr))) >>
                    shamt(instr));
                state.PC += 4;
              },
          },
          {
              InstructionEnum::ADD,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 + _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SUB,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 - _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SLL,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 << _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SLT,
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
              InstructionEnum::SLTU,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 < _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::XOR,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 ^ _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SRL,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 >> _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::SRA,
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
              InstructionEnum::OR,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 | _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::AND,
              [](MachineState &state, const uint32_t instr) -> void {
                auto &_rd = state.registers.at(rd(instr));
                auto _rs1 = state.registers.at(rs1(instr));
                auto _rs2 = state.registers.at(rs2(instr));
                _rd = _rs1 & _rs2;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::FENCE,
              [](MachineState &state, const uint32_t) -> void {
                state.PC += 4;
              },
          },
          {
              InstructionEnum::ECALL,
              [](MachineState &state, const uint32_t) -> void {
                throw rv_traps::Traps::Environment_call_from_M_mode;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::EBREAK,
              [](MachineState &state, const uint32_t) -> void {
                throw rv_traps::Traps::Breakpoint;
                state.PC += 4;
              },
          },
          {
              InstructionEnum::CSRRW,
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
              InstructionEnum::CSRRS,
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
              InstructionEnum::CSRRC,
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
              InstructionEnum::CSRRWI,
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
              InstructionEnum::CSRRSI,
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
              InstructionEnum::CSRRCI,
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
          {InstructionEnum::WFI,
           [](MachineState &, const uint32_t) -> void { terminate = true; }},
          {InstructionEnum::INVALID,
           [](MachineState &, const uint32_t) -> void { std::terminate(); }}};
    }();

#endif