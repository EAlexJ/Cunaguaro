#ifndef CSR_FILE_H
#define CSR_FILE_H

#include "general.h"
#include <cstddef>
#include <cstdint>
enum class Riscv_Csrs {
  // Machine Information
  // mvendorid = 0xF11,
  // marchid = 0xF12,
  // mimpid = 0xF13,
  // mhartid = 0xF14,
  // mconfigptr = 0xF15,

  // Machine Trap Setup
  mstatus = 0x300,
  misa = 0x301,
  // medeleg = 0x302,
  // mideleg = 0x303,
  mie = 0x304,
  mtvec = 0x305,
  // mcounteren = 0x306,
  mstatush = 0x310,
  // medelegh = 0x312,

  // Machine Trap Handling
  // mscratch = 0x340,
  mepc = 0x341,
  mcause = 0x342,
  mtval = 0x343,
  mip = 0x344,
  // mtinst = 0x34A,
  // mtval2 = 0x34B,

  // Machine Counter/Timers
  mcycle = 0xB00,
  minstret = 0xB02,
  // mhpmcounter3 = 0xB03,
  // mhpmcounter4 = 0xB04,
  // ...
  // mhpmcounter31 = 0xB1F,
  mcycleh = 0xB80,
  minstreth = 0xB82,
  // mhpmcounter3h = 0xB83,
  // ...
  // mhpmcounter4h = 0xB84,
  // mhpmcounter31h = 0xB9F,

};
constexpr auto MIP_SSIP = 1 << 1;
constexpr auto MIP_MSIP = 1 << 3;
constexpr auto MIP_STIP = 1 << 5;
constexpr auto MIP_MTIP = 1 << 7;
constexpr auto MIP_SEIP = 1 << 9;
constexpr auto MIP_MEIP = 1 << 11;
constexpr auto MIP_LCOFIP = 1 << 13;

constexpr auto MIE_SSIE = 1 << 1;
constexpr auto MIE_MSIE = 1 << 3;
constexpr auto MIE_STIE = 1 << 5;
constexpr auto MIE_MTIE = 1 << 7;
constexpr auto MIE_SEIE = 1 << 9;
constexpr auto MIE_MEIE = 1 << 11;
constexpr auto MIE_LCOFIE = 1 << 13;
constexpr auto MIE_WRITABLE_MASK = MIE_MSIE | MIE_MTIE | MIE_MEIE;

constexpr auto MSTATUS_SIE = 1 << 1;
constexpr auto MSTATUS_MIE = 1 << 3;
constexpr auto MSTATUS_SPIE = 1 << 5;
constexpr auto MSTATUS_UBE = 1 << 6;
constexpr auto MSTATUS_MPIE = 1 << 7;
constexpr auto MSTATUS_SPP = 1 << 8;
constexpr auto MSTATUS_VS = 3 << 9;
constexpr auto MSTATUS_MPP = 3 << 11;
constexpr auto MSTATUS_FS = 3 << 13;
constexpr auto MSTATUS_XS = 3 << 15;
constexpr auto MSTATUS_MPRV = 1 << 17;
constexpr auto MSTATUS_SUM = 1 << 18;
constexpr auto MSTATUS_MXR = 1 << 19;
constexpr auto MSTATUS_TVM = 1 << 20;
constexpr auto MSTATUS_TW = 1 << 21;
constexpr auto MSTATUS_TSR = 1 << 22;
constexpr auto MSTATUS_SPELP = 1 << 23;
constexpr auto MSTATUS_SDT = 1 << 24;
constexpr auto MSTATUS_SD = 1 << 31;
constexpr auto MSTATUS_WRITABLE_MASK = MSTATUS_MIE | MSTATUS_MPIE;
constexpr auto MSTATUS_HARDWIRED = MSTATUS_MPP;

constexpr auto MSTATUSH_SBE = 1 << 4;
constexpr auto MSTATUSH_MBE = 1 << 5;
constexpr auto MSTATUSH_GVA = 1 << 6;
constexpr auto MSTATUSH_MPV = 1 << 7;
constexpr auto MSTATUSH_MDELP = 1 << 9;
constexpr auto MSTATUSH_MDT = 1 << 10;
constexpr auto MSTATUSH_WRITABLE_MASK = 0;

template <typename reg_t, size_t IALIGN = 32> class SwitchCaseCSRs {
  struct {
    uint64_t mstatus = 0;
    //          11->RV64   ZXYWVUTSRQPONMLKJIHGFEDCBA
    //          01->RV32   --X-VU-S-Q---M---IH-FEDCBA
    reg_t misa = 0b01'0000'00000000000000000100000000;
    reg_t mie = 0;
    reg_t mtvec = 0;
    reg_t mepc = 0;
    reg_t mcause = 0;
    reg_t mtval = 0;
    reg_t mip = 0;
    uint64_t mcycle = 0;
    uint64_t minstret = 0;
  } backing;

public:
  uint64_t &mcycle = backing.mcycle;
  uint64_t &minstret = backing.minstret;
  reg_t &mtval = backing.mtval;
  reg_t &mepc = backing.mepc;
  reg_t &mcause = backing.mcause;
  reg_t &mtvec = backing.mtvec;

  [[nodiscard]] status read(const auto csr_id, reg_t &val) {
    switch ((static_cast<Riscv_Csrs>(csr_id))) {
      using enum Riscv_Csrs;
    case mstatus: {
      val = static_cast<reg_t>(backing.mstatus);
      break;
    }
    case misa: {
      val = backing.misa;
      break;
    }
    case mie: {
      val = backing.mie;
      break;
    }
    case mtvec: {
      val = backing.mtvec;
      break;
    }
    case mstatush: {
      val = static_cast<reg_t>(backing.mstatus >> 32);
      break;
    }
    case mepc: {
      val = backing.mepc;
      break;
    }
    case mcause: {
      val = backing.mcause;
      break;
    }
    case mtval: {
      val = backing.mtval;
      break;
    }
    case mip: {
      val = backing.mip;
      break;
    }
    case mcycle: {
      val = static_cast<reg_t>(backing.mcycle);
      break;
    }
    case minstret: {
      val = static_cast<reg_t>(backing.minstret);
      break;
    }
    case mcycleh: {
      val = static_cast<reg_t>(backing.mcycle >> 32);
      break;
    }
    case minstreth: {
      val = static_cast<reg_t>(backing.minstret >> 32);
      break;
    }
    default:
      return status::Error;
    }
    return status::Ok;
  };

  [[nodiscard]] status write(auto csr_id, const reg_t val) {
    switch ((static_cast<Riscv_Csrs>(csr_id))) {
      using enum Riscv_Csrs;
    case mstatus: {
      backing.mstatus = (val & MSTATUS_WRITABLE_MASK) | MSTATUS_HARDWIRED;
    }
    case misa: {
      break;
    }
    case mie: {
      backing.mie = val & MIE_WRITABLE_MASK;
      break;
    }
    case mtvec: {
      backing.mtvec = val;
      break;
    }
    case mstatush:
      break;
    case mepc: {
      constexpr reg_t mask =
          IALIGN == 32 ? static_cast<reg_t>(~0b11) : static_cast<reg_t>(~0b1);
      backing.mepc = val & mask;
      break;
    };
    case mcause: {
      backing.mcause = val;
      break;
    }
    case mtval: {
      backing.mtval = val;
      break;
    }
    case mip: {
      backing.mip = val;
      break;
    }
    case mcycle: {
      backing.mcycle = (backing.mcycle & 0xFFFFFFFF00000000) | val;
      break;
    }
    case minstret: {
      backing.minstret = (backing.minstret & 0xFFFFFFFF00000000) | val;
      break;
    }
    case mcycleh: {
      backing.mcycle =
          (backing.mcycle & 0xFFFFFFFF) | static_cast<uint64_t>(val) << 32;
      break;
    }
    case minstreth: {
      backing.minstret =
          (backing.minstret & 0xFFFFFFFF) | static_cast<uint64_t>(val) << 32;
      break;
    }
    default:
      return status::Error;
    }
    return status::Ok;
  };
};
#endif