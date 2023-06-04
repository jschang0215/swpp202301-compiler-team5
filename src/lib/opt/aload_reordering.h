#ifndef SWPP_ALOAD_REORDERING
#define SWPP_ALOAD_REORDERING

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <regex>

using namespace llvm;

class AloadReorderingPass : public PassInfoMixin<AloadReorderingPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

private:
  bool isAload(Instruction *I);
  bool isMalloc(Instruction *I);
  bool isFree(Instruction *I);
  int checkStackHeap(CallInst *CI);
  int getAloadCost(CallInst *CI);
};

#endif