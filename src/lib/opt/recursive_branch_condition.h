#ifndef SWPP_RECURSIVE_BRANCH_CONDITION
#define SWPP_RECURSIVE_BRANCH_CONDITION

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include <set>

using namespace llvm;

namespace LoopBranch {

// optimize recursive condition
class RecursiveBranchConditionPass
    : public PassInfoMixin<RecursiveBranchConditionPass> {
  using BlockSet = std::set<BasicBlock *>;

  struct RecursiveInfo {
    int cnt;
    bool reachable;
  };

  inline void insertSelect(LLVMContext &ctx, BranchInst *br);
  inline void invertCmp(BranchInst *br, CmpInst *cmp);
  inline bool checkInvertCondition(Value *cond);
  bool optimizeBrBlock(BasicBlock &BB);

  inline bool isRecursionBlock(BasicBlock &BB);
  inline bool isMoreRecursive(BasicBlock *BBt, BasicBlock *BBf);
  RecursiveInfo getRecursiveCnt(BasicBlock *BB1, BasicBlock *BB2);

  void recalculateRecursionBlocks(Function &F);

  BlockSet recursionBlocks;
  DominatorTree *DT;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

} // namespace LoopBranch

#endif