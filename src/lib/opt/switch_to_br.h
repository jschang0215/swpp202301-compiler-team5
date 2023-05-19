#ifndef SWPP_SWITCH_BR_CONVERT
#define SWPP_SWITCH_BR_CONVERT

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Passes/PassBuilder.h"
#include <set>

using namespace llvm;

namespace SwitchBr {
// optimize branch condition
class SwitchToBrPass : public PassInfoMixin<SwitchToBrPass> {
  using InstructionSet = std::set<Instruction *>;
  bool switchToBr(SwitchInst *inst);
  void replaceWithUncondBr(SwitchInst *inst);
  void replaceWithCondBr(SwitchInst *inst);
  InstructionSet eraseInst;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};
} // namespace SwitchBr

#endif