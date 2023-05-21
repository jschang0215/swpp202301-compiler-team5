#ifndef SWPP_SWITCH_IF_CONVERT
#define SWPP_SWITCH_IF_CONVERT

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <set>
#include <vector>
using namespace llvm;

namespace SwitchBr {
// optimize branch condition
class BrToSwitchPass : public PassInfoMixin<BrToSwitchPass> {
  using BlockSet = std::set<BasicBlock *>;
  using BrSet = std::set<BranchInst *>;
  struct BlockPair {
    BasicBlock *dest, *cond;
  };
  using BlockPairMap = std::map<ConstantInt *, BlockPair>;

  struct SwitchCaseInfo {
    Value *V;
    ConstantInt *C;
    BasicBlock *cond, *next;
  };
  struct CondInfo {
    Value *V;
    ConstantInt *C;
    bool EQ;
  };

  CondInfo getValueCond(ICmpInst *cond);
  SwitchCaseInfo getSwitchCase(BasicBlock *BB, Value *V);

  BasicBlock *addBridgeBB(BasicBlock *cond, BasicBlock *dest);
  BasicBlock *mergeBB(BasicBlock *base, BasicBlock *cond, BasicBlock *dest);
  bool makeSwitch(BasicBlock *base, Value *V, BlockPair def,
                  BlockPairMap &BBps);
  bool brToSwitch(BasicBlock *BB);
  void getLoopBr(Function &F, FunctionAnalysisManager &FAM);

  BlockSet eraseBB;
  BrSet loopBr;

  Function *F;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};
} // namespace SwitchBr

#endif