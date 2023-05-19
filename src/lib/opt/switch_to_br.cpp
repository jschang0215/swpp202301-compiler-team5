#include "switch_to_br.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"

using namespace SwitchBr;

/*
 * replace one case switch to conditional branch
 *
 * @inst:     target switch instruction
 */
void SwitchToBrPass::replaceWithCondBr(SwitchInst *inst) {
  BasicBlock *BBd = inst->getDefaultDest(),
             *BBv = inst->case_begin()->getCaseSuccessor();
  IRBuilder<> builder(inst);

  Value *cond = builder.CreateICmpEQ(inst->case_begin()->getCaseValue(),
                                     inst->getCondition());
  builder.CreateCondBr(cond, BBv, BBd);
  eraseInst.insert(inst);
}

/*
 * replace two case switch to conditional branch
 *
 * @inst:     target switch instruction
 */
void SwitchToBrPass::replaceWithUncondBr(SwitchInst *inst) {
  IRBuilder<> builder(inst);
  builder.CreateBr(inst->getDefaultDest());
  eraseInst.insert(inst);
}

/*
 * replace few case switch to branch
 *
 * @inst:     target switch instruction
 */
bool SwitchToBrPass::switchToBr(SwitchInst *inst) {
  if (inst->getNumCases() == 0)
    replaceWithUncondBr(inst);
  else if (inst->getNumCases() == 1)
    replaceWithCondBr(inst);
  return inst->getNumCases() <= 1;
}

PreservedAnalyses SwitchToBrPass::run(Function &F,
                                      FunctionAnalysisManager &FAM) {
  bool changed = false;
  eraseInst.clear();
  for (BasicBlock &BB : F)
    for (Instruction &I : BB) {
      SwitchInst *inst = dyn_cast<SwitchInst>(&I);
      if (inst)
        changed |= switchToBr(inst);
    }
  for (Instruction *I : eraseInst)
    I->eraseFromParent();
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SwitchToBrPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "SwitchToBrPass") {
                    FPM.addPass(SwitchToBrPass());
                    return true;
                  }
                  return false;
                });
          }};
}
