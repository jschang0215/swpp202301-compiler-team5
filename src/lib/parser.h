#ifndef SC_LIB_PARSER_H
#define SC_LIB_PARSER_H

/**
 * @file parser.h
 * @author SWPP TAs (swpp@sf.snu.ac.kr)
 * @brief Wrapper module for LLVM IR parser
 * @version 2023.1.0
 * @date 2023-04-16
 * @copyright Copyright (c) 2022-2023 SWPP TAs
 */

#include "../result.h"
#include "../static_error.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <memory>
#include <string>

namespace sc::parser {
/**
 * @brief Exception thrown by parser module
 */
class ParserError : public Error<ParserError> {
private:
  std::string message;

public:
  /**
   * @brief Construct a new ParserError object
   * @param message Message to show upon failure
   */
  ParserError(const std::string_view message) noexcept;
  /**
   * @brief Read the exception
   * @return Exception message in C-String format
   */
  const char *what() const noexcept { return message.c_str(); }
};

/**
 * @brief Parse the LLVM IR program string
 * 
 * This function parses the LLVM IR program string into llvm::Module using the
 * LLVM API, and returns error if the API fails to parse the program
 * 
 * @param code String that contains 'supposedly' a valid LLVM IR program
 * @param filename Origin of the code
 * @param context A fresh LLVMContext to be filled by parser
 * @return Result<std::unique_ptr<llvm::Module>, ParserError> A valid
 * llvm::Module if parsing succeeds, otherwise an error that contains the
 * reason of failure
 */
Result<std::unique_ptr<llvm::Module>, ParserError>
parseIR(const std::string_view code, const std::string_view filename,
        llvm::LLVMContext &context) noexcept;
} // namespace sc::parser
#endif // SC_LIB_PARSER_H
