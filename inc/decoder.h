#ifndef DECODER_H
#define DECODER_h
#include "instructions.h"
#include "machine.h"
#include "traps.h"
#include <ranges>
struct Decoder {
  static size_t decode(const reg_t instr) {
    for (const auto &[index, entry] :
         std::views::zip(std::views::iota(0), instructions)) {
      if ((instr & entry.mask) == entry.value)
        return static_cast<size_t>(index);
    }
    throw rv_traps::Traps::Illegal_instruction;
  }
};
#endif