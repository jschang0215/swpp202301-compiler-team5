#ifndef SWPP_MALLOC_FREE_REORDERING
#define SWPP_MALLOC_FREE_REORDERING

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include <map>

using namespace llvm;

class MallocFreeReorderingPass : public PassInfoMixin<MallocFreeReorderingPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

private:
};

#endif