#include "likely_branch_condition.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"

using namespace BranchLikely;

/*
 * Insert condition inversion after branch
 *
 * @br:     target branch instruction
 */
void LikelyBranchConditionPass::insertInversion(BranchInst *br) {
  IRBuilder<> builder(br);
  Value *cond = br->getCondition();
  Value *inversion = builder.CreateSelect(
      cond,
      ConstantInt::getBool(IntegerType::getInt1Ty(F->getContext()), false),
      ConstantInt::getBool(IntegerType::getInt1Ty(F->getContext()), true));
  br->setCondition(inversion);
  br->swapSuccessors();
}

/*
 * Swap branch successors with inverting condition
 *
 * @br:     target branch instruction
 */
void LikelyBranchConditionPass::swapBrSuccessors(BranchInst *br) {
  CmpInst *cmp = dyn_cast<CmpInst>(br->getCondition());
  // if cmp instruction inversion is not possible
  if (!cmp || cmp->getNumUses() > 1) {
    insertInversion(br);
    return;
  }
  cmp->setPredicate(cmp->getInversePredicate());
  br->swapSuccessors();
}

/*
 * Optimize branch terminator if possible
 *
 * @BB:     target basic block
 * @return: whether optimized or not
 */
bool LikelyBranchConditionPass::optimizeCondition(BasicBlock *BB) {
  BranchInst *br = dyn_cast<BranchInst>(BB->getTerminator());
  BrLikely info = brInfo.getBrLikely(br);

  outs() << ((info.first)?info.first->getName():"null") << " " << info.second << "\n";

  if (!br || !info.first || !info.second)
    return false;
  swapBrSuccessors(br);
  return true;
}

/*
 * Do likely branch condition optimize
 *
 * @F:      target function
 * @FAM:     function analysis manager
 */
PreservedAnalyses LikelyBranchConditionPass::run(Function &F,
                                                 FunctionAnalysisManager &FAM) {
  bool changed = false;
  this->F = &F;
  brInfo.recalculate(F, FAM);
  for (BasicBlock &BB : F)
    changed |= optimizeCondition(&BB);
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LikelyBranchConditionPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LikelyBranchConditionPass") {
                    FPM.addPass(LikelyBranchConditionPass());
                    return true;
                  }
                  return false;
                });
          }};
}
