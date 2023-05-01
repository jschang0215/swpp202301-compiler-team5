#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/STLExtras.h"

using namespace llvm;

class SimplePass : public PassInfoMixin<SimplePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

class LoadToAloadPass : public PassInfoMixin<LoadToAloadPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};