#ifndef SWPP_ALOAD_BLOCK
#define SWPP_ALOAD_BLOCK

#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <regex>
#include <set>
#include <vector>

using namespace llvm;

class AloadBlockPass : public PassInfoMixin<AloadBlockPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

private:
  bool isAload(Instruction *I);
  bool moveAload(BasicBlock *BB1, BasicBlock *BB2);
  bool reduceAload(BasicBlock *BB1, BasicBlock *BB2, BasicBlock *BB3);
};

#endif