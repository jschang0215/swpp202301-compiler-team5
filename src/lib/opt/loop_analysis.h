#ifndef SWPP_LOOP_ANALYSIS
#define SWPP_LOOP_ANALYSIS

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>

using namespace llvm;
using namespace std;

namespace LoopBranch {

class Loops {
  map<BasicBlock *, set<BasicBlock *>> loops;
  map<BasicBlock *, set<BasicBlock *>> inside_of;
  void findSimpleLoop(BasicBlock *BBh, BasicBlock *now,
                      set<BasicBlock *> &blocks, DominatorTree &DT);
  void addSimpleLoop(BasicBlock *BBh, BasicBlock *BBt, DominatorTree &DT);

public:
  Loops() = default;
  explicit Loops(Function &F, FunctionAnalysisManager &FAM) {
    recalculate(F, FAM);
  }
  void recalculate(Function &F, FunctionAnalysisManager &FAM);
  set<BasicBlock *> getLoop(BasicBlock *BBh);
  set<BasicBlock *> containigLoop(BasicBlock *BB);
  set<pair<BranchInst *, bool>> getExitBranches(BasicBlock *BBh);
  void print(raw_ostream &OS);
};

class LoopAnalysis : public AnalysisInfoMixin<LoopAnalysis> {
  friend AnalysisInfoMixin<LoopAnalysis>;
  static AnalysisKey Key;

public:
  using Result = Loops;
  Loops run(Function &F, FunctionAnalysisManager &FAM);
};

class LoopsPrinterPass : public PassInfoMixin<LoopsPrinterPass> {
  raw_ostream &OS;

public:
  explicit LoopsPrinterPass(raw_ostream &OS);

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

} // namespace LoopBranch

#endif