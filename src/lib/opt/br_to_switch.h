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
  struct BlockPair {
    BasicBlock *BBm, *BBc;
  }; // BB matched, BB condition
  using BlockPairMap = std::map<ConstantInt *, BlockPair>;

  struct SwitchCaseInfo {
    Value *V;
    ConstantInt *C;
    BasicBlock *BBc, *BBn;
  };
  struct CondInfo {
    Value *V;
    ConstantInt *C;
    bool EQ;
  };

  CondInfo getValueCond(ICmpInst *cond);
  SwitchCaseInfo getSwitchCase(BasicBlock *BB, Value *V);

  BasicBlock *addBridgeBB(BasicBlock *BBc, BasicBlock *BBm,
                          bool eraseBBc = true);
  bool tryMergeBB(BasicBlock *BBb, BasicBlock *BBc, BasicBlock *BBm);
  BasicBlock *makeDefaultCase(BasicBlock *BBb, BasicBlock *BBd,
                              BasicBlock *BBc);
  bool makeSwitch(BasicBlock *BBb, Value *V, BlockPair BBd, BlockPairMap &BBps);
  bool brToSwitch(BasicBlock *BB);

  BlockSet eraseBB;

  Function *F;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};
} // namespace SwitchBr

#endif