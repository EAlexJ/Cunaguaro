#ifndef OPCODE_H
#define OPCODE_H
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
constexpr bool isJump(Opcode op) {
  switch (op) {
    using enum Opcode;
  case JAL:
  case JALR:
  case ECALL:
  case EBREAK:
    return true;
  default:
    return false;
  }
}
constexpr bool isBranch(Opcode op) {
  switch (op) {
    using enum Opcode;
  case BEQ:
  case BGE:
  case BGEU:
  case BLT:
  case BLTU:
  case BNE:
    return true;
  default:
    return false;
  }
}
constexpr bool changesControlFlow(Opcode op) {
  return isJump(op) || isBranch(op);
}
#endif