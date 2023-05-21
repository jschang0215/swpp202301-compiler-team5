#ifndef SWPP_BRANCH_ANALYSIS
#define SWPP_BRANCH_ANALYSIS

#include "loop_analysis.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>

using namespace llvm;

namespace BranchLikely {
using BrLikely = std::pair<BranchInst*, bool>;
using BrSet = std::set<BrLikely>;
using BlockSet = std::set<BasicBlock *>;

class RecursiveBrInfo {
  struct RecursiveInfo {
    int cnt;
    bool reachable;
  };
  inline bool isRecursionBlock(BasicBlock &BB);
  void recalculateRecursionBlocks(Function &F);
  inline bool isMoreRecursive(BasicBlock *BBt, BasicBlock *BBf);
  RecursiveInfo getRecursiveCnt(BasicBlock *BB1, BasicBlock *BB2);
  void addRecursiveBrLikely(BasicBlock &BB);
  BlockSet recursionBlocks;
  DominatorTree *DT;
  BrSet brSet;
public:
  BrSet getBrSet();
  void recalculate(Function &F, FunctionAnalysisManager &FAM);
};

class BrLikelyInfo {
  void findRecursiveLikely(Function &F, FunctionAnalysisManager &FAM);
  void findLoopLikely(Function &F, FunctionAnalysisManager &FAM);
  BrSet brSet;
public:
  BrLikelyInfo() = default;
  explicit BrLikelyInfo(Function &F, FunctionAnalysisManager &FAM){
    recalculate(F, FAM);
  }
  void recalculate(Function &F, FunctionAnalysisManager &FAM);
  void print(raw_ostream &OS);
};

class BrLikelyPrinterPass : public PassInfoMixin<BrLikelyPrinterPass> {
  raw_ostream &OS;
public:
  explicit BrLikelyPrinterPass(raw_ostream &OS);
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

} // namespace LoopBranch

#endif