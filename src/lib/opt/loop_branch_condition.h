#ifndef SWPP_LOOP_BRANCH_CONDITION
#define SWPP_LOOP_BRANCH_CONDITION

#include "./loop_analysis.h"
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

namespace LoopBranch {

// optimize branch condition
class LoopBranchConditionPass : public PassInfoMixin<LoopBranchConditionPass> {
  void insertInversion(LLVMContext &ctx, BranchInst *br);
  map<Value *, set<pair<BranchInst *, bool>>> exit_conditions;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

} // namespace LoopBranch

#endif