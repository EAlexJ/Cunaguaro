#ifndef LOADER_H
#define LOADER_H

#include "elf.h"
#include "memory/memory.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <map>
#include <stdexcept>
#include <vector>
class MemoryMappedFile {
public:
  size_t size;
  void *ptr;
  MemoryMappedFile(std::filesystem::path path);
  MemoryMappedFile(const MemoryMappedFile &) = delete;
  MemoryMappedFile(const MemoryMappedFile &&) = delete;
  ~MemoryMappedFile();
};

template <MemoryTarget TargetMemory>
auto load_elf_file(TargetMemory &underlying, std::filesystem::path path,
                   std::map<std::string, uint64_t> &symbols) {
  auto loaded_file = MemoryMappedFile(path);
  std::byte *fptr = static_cast<std::byte *>(loaded_file.ptr);
  assert(loaded_file.size >= sizeof(Elf64_Ehdr));
  const Elf64_Ehdr *eh64 = reinterpret_cast<Elf64_Ehdr *>(fptr);
  assert(IS_ELF32(*eh64) || IS_ELF64(*eh64));
  unsigned xlen = IS_ELF32(*eh64) ? 32 : 64;
  if (xlen != 32)
    throw std::runtime_error(
        "Invalid XLEN for supplied ELF, this simulator only supports RV32");
  assert(IS_ELFLE(*eh64));
  assert(IS_ELF_EXEC(*eh64));
  assert(IS_ELF_RISCV(*eh64) || IS_ELF_EM_NONE(*eh64));
  assert(IS_ELF_VCURRENT(*eh64));

  std::vector<uint8_t> zeros;

  Elf32_Ehdr *eh = reinterpret_cast<Elf32_Ehdr *>(fptr);
  Elf32_Phdr *ph = reinterpret_cast<Elf32_Phdr *>(fptr + eh->e_phoff);
  auto entry = eh->e_entry;
  assert(loaded_file.size >= eh->e_phoff + eh->e_phnum * sizeof(*ph));
  for (unsigned i = 0; i < eh->e_phnum; i++) {
    if ((ph)[i].p_type == PT_LOAD && (ph)[i].p_memsz) {
      uint64_t load_addr = (ph)[i].p_paddr;
      if ((ph)[i].p_filesz) {
        assert(loaded_file.size >= (ph)[i].p_offset + (ph)[i].p_filesz);
        auto status = underlying.write(
            load_addr, {fptr + (ph)[i].p_offset, (ph)[i].p_filesz});
        if (status != status::Ok) {
          throw std::runtime_error("ELF loading went wrong");
        }
      }
      if (size_t pad = (ph)[i].p_memsz - (ph)[i].p_filesz) {
        zeros.resize(pad);
        auto status = underlying.write(
            load_addr + (ph)[i].p_filesz,
            {reinterpret_cast<std::byte *>(zeros.data()), pad});
        if (status != status::Ok) {
          throw std::runtime_error("ELF loading went wrong");
        }
      }
    }
  }
  Elf32_Shdr *sh = reinterpret_cast<Elf32_Shdr *>(fptr + eh->e_shoff);
  assert(loaded_file.size >= eh->e_shoff + eh->e_shnum * sizeof(*sh));
  assert(eh->e_shstrndx < eh->e_shnum);
  assert(loaded_file.size >=
         (sh)[eh->e_shstrndx].sh_offset + (sh)[eh->e_shstrndx].sh_size);
  char *shstrtab =
      reinterpret_cast<char *>(fptr + (sh)[eh->e_shstrndx].sh_offset);
  unsigned strtabidx = 0, symtabidx = 0;
  for (unsigned i = 0; i < eh->e_shnum; i++) {
    [[maybe_unused]]
    unsigned max_len = (sh)[eh->e_shstrndx].sh_size - (sh)[i].sh_name;
    assert((sh)[i].sh_name < (sh)[eh->e_shstrndx].sh_size);
    assert(strnlen(shstrtab + (sh)[i].sh_name, max_len) < max_len);
    if ((sh)[i].sh_type & SHT_NOBITS)
      continue;
    assert(loaded_file.size >= (sh)[i].sh_offset + (sh)[i].sh_size);
    if (strcmp(shstrtab + (sh)[i].sh_name, ".strtab") == 0)
      strtabidx = i;
    if (strcmp(shstrtab + (sh)[i].sh_name, ".symtab") == 0)
      symtabidx = i;
  }
  if (strtabidx && symtabidx) {
    char *strtab = reinterpret_cast<char *>(fptr + (sh)[strtabidx].sh_offset);
    Elf32_Sym *sym =
        reinterpret_cast<Elf32_Sym *>(fptr + sh[symtabidx].sh_offset);
    for (unsigned i = 0; i < (sh)[symtabidx].sh_size / sizeof(Elf32_Sym); i++) {
      [[maybe_unused]]
      unsigned max_len = (sh)[strtabidx].sh_size - (sym)[i].st_name;
      assert((sym)[i].st_name < (sh)[strtabidx].sh_size);
      assert(strnlen(reinterpret_cast<char *>(strtab + (sym)[i].st_name),
                     max_len) < max_len);
      symbols[strtab + (sym)[i].st_name] = (sym)[i].st_value;
    }
  }
  return entry;
}
#endif