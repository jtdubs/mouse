#include "symbols.hh"

#include <cxxabi.h>

#include <elfio/elfio.hpp>

namespace sim {

std::vector<Symbol> ReadSymbols(avr_t* avr, std::string firmware_path) {
  std::vector<Symbol> result;

  ELFIO::elfio reader;
  if (!reader.load(firmware_path)) {
    fprintf(stderr, "elfio::load failed\n");
    return result;
  }

  for (const auto& sec : reader.sections) {
    if (sec->get_type() != ELFIO::SHT_SYMTAB && sec->get_type() != ELFIO::SHT_DYNSYM) {
      continue;
    }

    ELFIO::const_symbol_section_accessor symbols(reader, sec.get());

    ELFIO::Elf_Xword sym_no = symbols.get_symbols_num();
    if (sym_no == 0) {
      continue;
    }

    printf("Symbol table (%s)\n", sec->get_name().c_str());
    for (ELFIO::Elf_Xword i = 0; i < sym_no; i++) {
      std::string       name          = "";
      ELFIO::Elf64_Addr value         = 0;
      ELFIO::Elf_Xword  size          = 0;
      unsigned char     bind          = 0;
      unsigned char     type          = 0;
      ELFIO::Elf_Half   section_index = 0;
      unsigned char     other         = 0;

      symbols.get_symbol(i, name, value, size, bind, type, section_index, other);
      if (type != ELFIO::STT_OBJECT) {
        continue;
      }
      if (bind != ELFIO::STB_LOCAL) {
        continue;
      }

      char* demangled = __cxxabiv1::__cxa_demangle(name.c_str(), nullptr, nullptr, nullptr);
      if (demangled == nullptr) {
        result.push_back(Symbol{name, value, size});
      } else {
        std::string d(demangled);
        if (size_t idx = d.find("::(anonymous namespace)"); idx != std::string::npos) {
          d = d.replace(idx, d.find("::", idx + 2) - idx, "");
        }
        if (size_t idx = d.find("("); idx != std::string::npos) {
          d = d.replace(idx, d.find(")", idx), "");
        }
        result.push_back(Symbol{
            .name    = d,
            .address = value - 0x800000,
            .size    = size,
            .data    = &avr->data[value - 0x800000],
        });
        free(demangled);
      }
    }
  }

  return result;
}

}  // namespace sim
