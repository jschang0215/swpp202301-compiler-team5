#include "aload_block.h"

/*
 * check instruction is aload
 *
 * @I:     instruction to check
 * return: true if I is aload
 */
bool AloadBlockPass::isAload(Instruction *I) {
  std::regex aload_pattern("aload_i(8|16|32|64)");
  if (CallInst *CI = dyn_cast<CallInst>(I)) {
    StringRef functionName = CI->getCalledFunction()->getName();
    if (std::regex_match(functionName.str(), aload_pattern))
      return true;
  }
  return false;
}

/*
 * move aload instructions in BB2 to BB1
 * BB1 must dominate BB2
 *
 * @BB1:   target basicblock
 * @BB2:   source basicblock that contains I
 * return: true if moved
 */
bool AloadBlockPass::moveAload(BasicBlock *BB1, BasicBlock *BB2) {
  bool changed = false;
  Instruction *terminator = BB1->getTerminator();
  std::vector<Instruction *> aloads;
  for (Instruction &I : *BB2) {
    if (isAload(&I)) {
      aloads.push_back(&I);
    } else {
      break;
    }
  }
  for (Instruction *I : aloads) {
    I->moveBefore(terminator);
    changed = true;
  }
  return changed;
}

/*
 * move aloads use same pointer in BB2, BB3 to BB1
 * BB1 must dominate BB2, BB3
 * if instructions in different block use same pointer,
 * don't need to check dependency
 *
 * @BB1:   target basicblock
 * @BB2:   true basicblock
 * @BB3:   false basicblock
 * return: true if moved
 */

bool AloadBlockPass::reduceAload(BasicBlock *BB1, BasicBlock *BB2,
                                 BasicBlock *BB3) {
  bool changed = false;
  Instruction *terminator = BB1->getTerminator();
  std::vector<Instruction *> aloads1;
  std::vector<Instruction *> aloads2;
  std::set<CallInst *> remove;
  for (Instruction &I : *BB2) {
    if (isAload(&I))
      aloads1.push_back(&I);
  }
  for (Instruction &I : *BB3) {
    if (isAload(&I))
      aloads2.push_back(&I);
  }
  for (Instruction *I1 : aloads1) {
    CallInst *CI1 = dyn_cast<CallInst>(I1);
    for (Instruction *I2 : aloads2) {
      CallInst *CI2 = dyn_cast<CallInst>(I2);
      if (CI1->getArgOperand(0) == CI2->getArgOperand(0)) {
        CI1->moveBefore(terminator);
        CI2->replaceAllUsesWith(CI1);
        remove.insert(CI2);
        changed = true;
      }
    }
  }
  for (CallInst *CI : remove) {
    CI->eraseFromParent();
  }
  return changed;
}

PreservedAnalyses AloadBlockPass::run(Function &F,
                                      FunctionAnalysisManager &FAM) {
  bool changed = false;
  std::vector<std::pair<BasicBlock *, BasicBlock *>> blockPairs;
  DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);

  for (BasicBlock &BB : F) {
    if (BranchInst *branch = dyn_cast<BranchInst>(BB.getTerminator())) {
      if (branch->isUnconditional()) {
        BasicBlock *nextBranch = branch->getSuccessor(0);
        if (DT.dominates(&BB, nextBranch)) {
          if (moveAload(&BB, nextBranch))
            changed = true;
        }
      } else if (branch->isConditional()) {
        BasicBlock *trueBranch = branch->getSuccessor(0);
        BasicBlock *falseBranch = branch->getSuccessor(1);
        if (DT.dominates(&BB, trueBranch) && DT.dominates(&BB, falseBranch)) {
          if (reduceAload(&BB, trueBranch, falseBranch))
            changed = true;
        }
      }
    }
  }
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AloadBlockPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "AloadBlockPass") {
                    FPM.addPass(AloadBlockPass());
                    return true;
                  }
                  return false;
                });
          }};
}