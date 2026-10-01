#ifndef DECODER_H
#define DECODER_H

#include "instructions.h"
#include "machine.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ranges>

namespace loop_decoder {
struct InstructionDescription {
  const uint32_t value;
  const uint32_t mask;
  const Opcode instr;
};

constexpr std::array<const InstructionDescription, instruction_count>
    instructions{{
        {0b00000000000000000000000000110111, 0b00000000000000000000000001111111,
         Opcode::LUI},
        {0b00000000000000000000000000010111, 0b00000000000000000000000001111111,
         Opcode::AUIPC},
        {0b00000000000000000000000001101111, 0b00000000000000000000000001111111,
         Opcode::JAL},
        {0b00000000000000000000000001100111, 0b00000000000000000111000001111111,
         Opcode::JALR},
        {0b00000000000000000000000001100011, 0b00000000000000000111000001111111,
         Opcode::BEQ},
        {0b00000000000000000001000001100011, 0b00000000000000000111000001111111,
         Opcode::BNE},
        {0b00000000000000000100000001100011, 0b00000000000000000111000001111111,
         Opcode::BLT},
        {0b00000000000000000101000001100011, 0b00000000000000000111000001111111,
         Opcode::BGE},
        {0b00000000000000000110000001100011, 0b00000000000000000111000001111111,
         Opcode::BLTU},
        {0b00000000000000000111000001100011, 0b00000000000000000111000001111111,
         Opcode::BGEU},
        {0b00000000000000000000000000000011, 0b00000000000000000111000001111111,
         Opcode::LB},
        {0b00000000000000000001000000000011, 0b00000000000000000111000001111111,
         Opcode::LH},
        {0b00000000000000000010000000000011, 0b00000000000000000111000001111111,
         Opcode::LW},
        {0b00000000000000000100000000000011, 0b00000000000000000111000001111111,
         Opcode::LBU},
        {0b00000000000000000101000000000011, 0b00000000000000000111000001111111,
         Opcode::LHU},
        {0b00000000000000000000000000100011, 0b00000000000000000111000001111111,
         Opcode::SB},
        {0b00000000000000000001000000100011, 0b00000000000000000111000001111111,
         Opcode::SH},
        {0b00000000000000000010000000100011, 0b00000000000000000111000001111111,
         Opcode::SW},
        {0b00000000000000000000000000010011, 0b00000000000000000111000001111111,
         Opcode::ADDI},
        {0b00000000000000000010000000010011, 0b00000000000000000111000001111111,
         Opcode::SLTI},
        {0b00000000000000000011000000010011, 0b00000000000000000111000001111111,
         Opcode::SLTIU},
        {0b00000000000000000100000000010011, 0b00000000000000000111000001111111,
         Opcode::XORI},
        {0b00000000000000000110000000010011, 0b00000000000000000111000001111111,
         Opcode::ORI},
        {0b00000000000000000111000000010011, 0b00000000000000000111000001111111,
         Opcode::ANDI},
        {0b00000000000000000001000000010011, 0b11111110000000000111000001111111,
         Opcode::SLLI},
        {0b00000000000000000101000000010011, 0b11111110000000000111000001111111,
         Opcode::SRLI},
        {0b01000000000000000101000000010011, 0b11111110000000000111000001111111,
         Opcode::SRAI},
        {0b00000000000000000000000000110011, 0b11111110000000000111000001111111,
         Opcode::ADD},
        {0b01000000000000000000000000110011, 0b11111110000000000111000001111111,
         Opcode::SUB},
        {0b00000000000000000001000000110011, 0b11111110000000000111000001111111,
         Opcode::SLL},
        {0b00000000000000000010000000110011, 0b11111110000000000111000001111111,
         Opcode::SLT},
        {0b00000000000000000011000000110011, 0b11111110000000000111000001111111,
         Opcode::SLTU},
        {0b00000000000000000100000000110011, 0b11111110000000000111000001111111,
         Opcode::XOR},
        {0b00000000000000000101000000110011, 0b11111110000000000111000001111111,
         Opcode::SRL},
        {0b01000000000000000101000000110011, 0b11111110000000000111000001111111,
         Opcode::SRA},
        {0b00000000000000000110000000110011, 0b11111110000000000111000001111111,
         Opcode::OR},
        {0b00000000000000000111000000110011, 0b11111110000000000111000001111111,
         Opcode::AND},
        {0b00000000000000000000000000001111, 0b00000000000000000111000001111111,
         Opcode::FENCE},
        {0b00000000000000000000000001110011, 0b11111111111111111111111111111111,
         Opcode::ECALL},
        {0b00000000000100000000000001110011, 0b11111111111111111111111111111111,
         Opcode::EBREAK},
        {0b00000000000000000001000001110011, 0b00000000000000000111000001111111,
         Opcode::CSRRW},
        {0b00000000000000000010000001110011, 0b00000000000000000111000001111111,
         Opcode::CSRRS},
        {0b00000000000000000011000001110011, 0b00000000000000000111000001111111,
         Opcode::CSRRC},
        {0b00000000000000000101000001110011, 0b00000000000000000111000001111111,
         Opcode::CSRRWI},
        {0b00000000000000000110000001110011, 0b00000000000000000111000001111111,
         Opcode::CSRRSI},
        {0b00000000000000000111000001110011, 0b00000000000000000111000001111111,
         Opcode::CSRRCI},
        {0b00010000010100000000000001110011, 0b11111111111111111111111111111111,
         Opcode::WFI},
    }};

struct Decoder {
  static Opcode decode(const reg_t instr) {
    for (const auto &entry : instructions) {
      if ((instr & entry.mask) == entry.value)
        return entry.instr;
    }
    throw rv_traps::Traps::Instruction_access_fault;
  }
};
} // namespace loop_decoder

namespace lut_decoder {
inline constexpr std::nullopt_t ANY_FUNCT7 = std::nullopt;
inline constexpr std::nullopt_t ANY_FUNCT3 = std::nullopt;
constexpr std::size_t get_instr_idx(const uint32_t k) {
  assert((k & 0b11) == 0b11);
  auto funct7_bit = bit_range<30, 30>(k);
  auto funct3 = bit_range<14, 12>(k);
  auto opcode_part = bit_range<6, 2>(k);
  return funct7_bit << 8 | funct3 << 5 | opcode_part;
}

struct InstructionCollector {
  std::array<Opcode, 512> entries = []() {
    std::array<Opcode, 512> entries;
    entries.fill(Opcode::ILLEGAL);
    return entries;
  }();
  consteval void add_encoding(Opcode instr, std::optional<uint8_t> funct7_bit,
                              std::optional<uint8_t> funct3,
                              uint8_t opcode_bits) {
    if ((opcode_bits & 0b11) != 0b11)
      throw "Found illegal opcode";
    if (funct7_bit.has_value() && funct7_bit.value() > 1)
      throw "Found illegal funct7_bit";
    if (funct3.has_value() && funct3.value() > 0b111)
      throw "Found illegal funct3";

    auto f7_range =
        funct7_bit == ANY_FUNCT7
            ? std::views::iota(0U, 2U)
            : std::views::iota(static_cast<uint32_t>(*funct7_bit),
                               static_cast<uint32_t>(*funct7_bit + 1));
    auto f3_range = funct3 == ANY_FUNCT3
                        ? std::views::iota(0U, 8U)
                        : std::views::iota(static_cast<uint32_t>(*funct3),
                                           static_cast<uint32_t>(*funct3 + 1));
    for (const auto f7 : f7_range)
      for (const auto f3 : f3_range) {
        uint32_t would_be_instruction = (f7 << 30) | (f3 << 12) | opcode_bits;
        auto encoding = get_instr_idx(would_be_instruction);
        assert(entries[encoding] == Opcode::ILLEGAL);
        entries.at(encoding) = instr;
      }
  };
};
consteval auto generate_instruction_lut() {
  constexpr auto LOAD = 0b0000011;
  constexpr auto STORE = 0b0100011;
  constexpr auto BRANCH = 0b1100011;
  constexpr auto OP_IMM = 0b0010011;
  constexpr auto OP = 0b0110011;
  constexpr auto SYSTEM = 0b1110011;
  constexpr auto MISC_MEM = 0b0001111;
  InstructionCollector collector;
  collector.add_encoding(Opcode::LUI, ANY_FUNCT7, ANY_FUNCT3, 0b0110111);
  collector.add_encoding(Opcode::AUIPC, ANY_FUNCT7, ANY_FUNCT3, 0b0010111);
  collector.add_encoding(Opcode::JAL, ANY_FUNCT7, ANY_FUNCT3, 0b1101111);
  collector.add_encoding(Opcode::JALR, ANY_FUNCT7, ANY_FUNCT3, 0b1100111);
  collector.add_encoding(Opcode::BEQ, ANY_FUNCT7, 0b000, BRANCH);
  collector.add_encoding(Opcode::BNE, ANY_FUNCT7, 0b001, BRANCH);
  collector.add_encoding(Opcode::BLT, ANY_FUNCT7, 0b100, BRANCH);
  collector.add_encoding(Opcode::BGE, ANY_FUNCT7, 0b101, BRANCH);
  collector.add_encoding(Opcode::BLTU, ANY_FUNCT7, 0b110, BRANCH);
  collector.add_encoding(Opcode::BGEU, ANY_FUNCT7, 0b111, BRANCH);
  collector.add_encoding(Opcode::LB, ANY_FUNCT7, 0b000, LOAD);
  collector.add_encoding(Opcode::LH, ANY_FUNCT7, 0b001, LOAD);
  collector.add_encoding(Opcode::LW, ANY_FUNCT7, 0b010, LOAD);
  collector.add_encoding(Opcode::LBU, ANY_FUNCT7, 0b100, LOAD);
  collector.add_encoding(Opcode::LHU, ANY_FUNCT7, 0b101, LOAD);
  collector.add_encoding(Opcode::SB, ANY_FUNCT7, 0b000, STORE);
  collector.add_encoding(Opcode::SH, ANY_FUNCT7, 0b001, STORE);
  collector.add_encoding(Opcode::SW, ANY_FUNCT7, 0b010, STORE);
  collector.add_encoding(Opcode::ADDI, ANY_FUNCT7, 0b000, OP_IMM);
  collector.add_encoding(Opcode::SLTI, ANY_FUNCT7, 0b010, OP_IMM);
  collector.add_encoding(Opcode::SLTIU, ANY_FUNCT7, 0b0011, OP_IMM);
  collector.add_encoding(Opcode::XORI, ANY_FUNCT7, 0b100, OP_IMM);
  collector.add_encoding(Opcode::ORI, ANY_FUNCT7, 0b110, OP_IMM);
  collector.add_encoding(Opcode::ANDI, ANY_FUNCT7, 0b111, OP_IMM);
  collector.add_encoding(Opcode::SLLI, 0b0, 0b001, OP_IMM);
  collector.add_encoding(Opcode::SRLI, 0b0, 0b101, OP_IMM);
  collector.add_encoding(Opcode::SRAI, 0b1, 0b101, OP_IMM);
  collector.add_encoding(Opcode::ADD, 0b0, 0b000, OP);
  collector.add_encoding(Opcode::SUB, 0b1, 0b000, OP);
  collector.add_encoding(Opcode::SLL, 0b0, 0b001, OP);
  collector.add_encoding(Opcode::SLT, 0b0, 0b010, OP);
  collector.add_encoding(Opcode::SLTU, 0b0, 0b011, OP);
  collector.add_encoding(Opcode::XOR, 0b0, 0b100, OP);
  collector.add_encoding(Opcode::SRL, 0b0, 0b101, OP);
  collector.add_encoding(Opcode::SRA, 0b1, 0b101, OP);
  collector.add_encoding(Opcode::OR, 0b0, 0b110, OP);
  collector.add_encoding(Opcode::AND, 0b0, 0b111, OP);
  collector.add_encoding(Opcode::FENCE, ANY_FUNCT7, 0b000, MISC_MEM);
  // ECALL, EBREAK and WFI
  collector.add_encoding(Opcode::SYSTEM_PRIV, 0b0, 0b000, SYSTEM);
  collector.add_encoding(Opcode::CSRRW, ANY_FUNCT7, 0b001, SYSTEM);
  collector.add_encoding(Opcode::CSRRS, ANY_FUNCT7, 0b010, SYSTEM);
  collector.add_encoding(Opcode::CSRRC, ANY_FUNCT7, 0b011, SYSTEM);
  collector.add_encoding(Opcode::CSRRWI, ANY_FUNCT7, 0b101, SYSTEM);
  collector.add_encoding(Opcode::CSRRSI, ANY_FUNCT7, 0b110, SYSTEM);
  collector.add_encoding(Opcode::CSRRCI, ANY_FUNCT7, 0b111, SYSTEM);

  return collector.entries;
}

class Decoder {
  static constexpr std::array<Opcode, 512> backing = generate_instruction_lut();

public:
  static Opcode decode(uint32_t instr) {
    auto decoded = backing[get_instr_idx(instr)];
    [[unlikely]]
    if (decoded == Opcode::SYSTEM_PRIV) {
      if (instr == 0b00000000'00000000'00000000'01110011)
        return Opcode::ECALL;
      if (instr == 0b00000001'00000000'00000000'01110011)
        return Opcode::EBREAK;
      if (instr == 0b00010000'01010000'00000000'01110011)
        return Opcode::WFI;
    }
    return decoded;
  }
};
} // namespace lut_decoder
#endif