#ifndef SC_LIB_OPT_H
#define SC_LIB_OPT_H

/**
 * @file opt.h
 * @author SWPP TAs (swpp@sf.snu.ac.kr)
 * @brief Optimization module
 * @version 2023.1.0
 * @date 2023-04-16
 * @copyright Copyright (c) 2022-2023 SWPP TAs
 */

#include "../result.h"
#include "../static_error.h"
#include "llvm/IR/PassManager.h"

#include <memory>
#include <string>

namespace sc::opt {
/**
 * @brief Exception thrown while applying the optimization pass
 */
class OptInternalError : public Error<OptInternalError> {
private:
  std::string message;

public:
  /**
   * @brief Construct a new OptInternalError object
   *
   * This is a type cast interface for exceptions thrown during the
   * optimization process.
   * @param e Any exception
   */
  OptInternalError(const std::exception &e) noexcept;

  /**
   * @brief Read the exception
   * @return Exception message in C-String format
   */
  const char *what() const noexcept { return message.c_str(); }
};

/**
 * @brief Optimize the LLVM IR program using the implemented optimization passes
 * 
 * This function applies the optimization passes to the given IR program in the
 * designated order, with help of various PassManagers.
 *
 * @param M IR program to optimize
 * @param MAM Reference to cross-registered ModuleAnalysisManager
 * @return Result<std::unique_ptr<llvm::Module>, OptInternalError> Optimized IR
 * program if none of the passes fail, otherwise an error from the failed pass
 */
Result<std::unique_ptr<llvm::Module>, OptInternalError>
optimizeIR(std::unique_ptr<llvm::Module> &&M,
           llvm::ModuleAnalysisManager &MAM) noexcept;
} // namespace sc::opt
#endif // SC_LIB_OPT_H
