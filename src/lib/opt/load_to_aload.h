#ifndef SWPP_LOAD_TO_ALOAD
#define SWPP_LOAD_TO_ALOAD

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <regex>

using namespace llvm;

namespace ToAload {

class LoadToAloadPass : public PassInfoMixin<LoadToAloadPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  int getCost(Instruction *I);

private:
  bool calcCost(LoadInst *LI);
  void loadChange(LoadInst *LI, IRBuilder<> &builder);
};

} // namespace ToAload
#endif