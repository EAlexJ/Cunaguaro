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
  SYSTEM_PRIV, // is ECALL, EBREAK, or WFI
};
constexpr bool isRandomJump(Opcode op) {
  switch (op) {
    using enum Opcode;
  case JALR:
  case ECALL:
  case EBREAK:
  case SYSTEM_PRIV:
    return true;
  default:
    return false;
  }
}
constexpr bool isJump(Opcode op) {
  return isRandomJump(op) || op == Opcode::JAL;
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
constexpr bool isDeterministicJump(Opcode op) { return !isRandomJump(op); }
#endif