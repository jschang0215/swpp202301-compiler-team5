#ifndef SC_LIB_BACKEND_ASSEMBLY_WIDTH_T_H
#define SC_LIB_BACKEND_ASSEMBLY_WIDTH_T_H

#include <string>

namespace sc::backend::assembly::width_t {
enum class AccessWidth {
  BYTE,  // 1B
  WORD,  // 2B
  DWORD, // 4B
  QWORD  // 8B
};

enum class BitWidth {
  BIT,   // 1bit
  BYTE,  // 8bit
  WORD,  // 16bit
  DWORD, // 32bit
  QWORD  // 64bit
};

std::string getToken(const AccessWidth width) noexcept;
std::string getToken(const BitWidth width) noexcept;
} // namespace sc::backend::assembly::width_t
#endif // SC_LIB_BACKEND_ASSEMBLY_WIDTH_T_H
