#ifndef SWPP_HEAP_PROMOTION
#define SWPP_HEAP_PROMOTION

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

class HeapPromotionPass : public PassInfoMixin<HeapPromotionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

#endif