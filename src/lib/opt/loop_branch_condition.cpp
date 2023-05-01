#include "loop_branch_condition.h"

using namespace LoopBranch;

/*
 * Insert condition inversion after branch
 *
 * @ctx:    LLVM Context
 * @br:     target branch instruction
 */
void LoopBranch::LoopBranchConditionPass::insertInversion(LLVMContext &ctx,
                                                          BranchInst *br) {
  outs() << "insert inversion\n";
  SelectInst *inversion = SelectInst::Create(
      br->getCondition(),
      ConstantInt::getBool(IntegerType::getInt1Ty(ctx), false),
      ConstantInt::getBool(IntegerType::getInt1Ty(ctx), true), "", br);
  br->setCondition(inversion);
  br->swapSuccessors();
}

/*
 * Do loop branch condition optimize
 *
 * when exit the loop, make branch instruction's conditions
 * as false, which decrease loop cost
 *
 * @F:      target function
 * @FAM:     function analysis manager
 */
PreservedAnalyses
LoopBranch::LoopBranchConditionPass::run(Function &F,
                                         FunctionAnalysisManager &FAM) {

  LoopBranch::LoopAnalysis a;
  LoopBranch::Loops loops = a.run(F, FAM);
  exit_conditions.clear();
  for (BasicBlock &BBh : F) { // get all exit condition...
    for (auto &[br, c] : loops.getExitBranches(&BBh)) {
      Value *cond = br->getCondition();
      if (exit_conditions.find(cond) == exit_conditions.end())
        exit_conditions[cond] = {};
      exit_conditions[cond].insert({br, c});
    }
  }

  for (auto &[value, uses] : exit_conditions) { // for all exit condition...
    CmpInst *cmp = dyn_cast<CmpInst>(value);
    int true_case = 0;
    for (auto &[br, c] : uses)
      true_case += c;

    // find inversion case
    // all usage is exit branch and exit when false
    if (cmp && cmp->getNumUses() == uses.size() && true_case == uses.size()) {
      cmp->setPredicate(cmp->getInversePredicate());
      for (auto &[br, c] : uses)
        br->swapSuccessors();
      continue;
    }

    for (auto &[br, c] : uses)
      if (c)
        insertInversion(F.getContext(), br);
  }

  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopBranchConditionPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LoopBranchConditionPass") {
                    FPM.addPass(LoopBranchConditionPass());
                    return true;
                  }
                  return false;
                });
          }};
}
