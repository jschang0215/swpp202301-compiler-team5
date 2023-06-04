#ifndef SWPP_LIKELY_BRANCH_CONDITION
#define SWPP_LIKELY_BRANCH_CONDITION

#include "./branch_likely_analysis.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace std;
using namespace llvm;

namespace BranchLikely {

// optimize branch condition
class LikelyBranchConditionPass
    : public PassInfoMixin<LikelyBranchConditionPass> {
  void insertInversion(BranchInst *br);
  void swapBrSuccessors(BranchInst *br);
  bool optimizeCondition(BasicBlock *BB);

  BrLikelyInfo brInfo;
  Function *F;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

} // namespace BranchLikely

#endif