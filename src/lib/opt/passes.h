#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class SimplePass : public PassInfoMixin<SimplePass> {
  public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

class LoadReorderingPass : public PassInfoMixin<LoadReorderingPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};