#include "aload_reordering.h"
#include "load_to_aload.h"

/*
 * check instruction is aload
 *
 * @I:     instruction to check
 * return: true if I is aload
 */
inline bool AloadReorderingPass::isAload(Instruction *I) {
  std::regex aload_pattern("aload_i(8|16|32|64)");
  if (CallInst *CI = dyn_cast<CallInst>(I)) {
    StringRef functionName = CI->getCalledFunction()->getName();
    if (std::regex_match(functionName.str(), aload_pattern))
      return true;
  }
  return false;
}

/*
 * check instruction is malloc
 *
 * @I:     instruction to check
 * return: true if I is malloc
 */
inline bool AloadReorderingPass::isMalloc(Instruction *I) {
  if (CallInst *CI = dyn_cast<CallInst>(I)) {
    StringRef functionName = CI->getCalledFunction()->getName();
    if (functionName == "malloc")
      return true;
  }
  return false;
}

/*
 * check instruction is free
 *
 * @I:     instruction to check
 * return: true if I is free
 */
inline bool AloadReorderingPass::isFree(Instruction *I) {
  if (CallInst *CI = dyn_cast<CallInst>(I)) {
    StringRef functionName = CI->getCalledFunction()->getName();
    if (functionName == "free")
      return true;
  }
  return false;
}

/*
 * check aload ptr is in heap or stack
 *
 * @CI:    aload instruction
 * return: 0 : stack, 1 : heap, -1 : cannot decide
 */
int AloadReorderingPass::checkStackHeap(CallInst *CI) {
  Value *ptr = CI->getArgOperand(0);

  // check pointer is used in call
  for (User *U : ptr->users()) {
    if (Instruction *I = dyn_cast<Instruction>(U)) {
      if (CallInst *CI = dyn_cast<CallInst>(I)) {
        if (!isAload(CI) && !isFree(CI))
          return -1;
      }
    }
  }
  if (Instruction *I = dyn_cast<Instruction>(ptr)) {
    if (AllocaInst *AI = dyn_cast<AllocaInst>(I))
      return 0;
    if (isMalloc(I))
      return 1;
  }
  return -1;
}

/*
 * calculate cost of aload instruction
 * not exact cost
 * if pointer in stack : just 24 - cost
 * if pointer in heap : just 34 - cost
 *
 * @CI:    aload instruction to calculate
 * return: cost, can be negative
 */
int AloadReorderingPass::getAloadCost(CallInst *CI) {
  ToAload::LoadToAloadPass pass;
  int cost = 0;
  Instruction *nextInst = CI->getNextNode();
  bool check = true;
  while (nextInst != nullptr && check) {
    for (auto &op : nextInst->operands()) {
      if (op.get() == CI)
        check = false;
    }
    if (check)
      cost += pass.getCost(nextInst);
    nextInst = nextInst->getNextNode();
  }
  int c = checkStackHeap(CI);
  if (c == 0)
    return 24 - cost;
  else if (c == 1)
    return 34 - cost;
  else
    return 1; // can not decide stack or heap, don't move
}

PreservedAnalyses AloadReorderingPass::run(Function &F,
                                           FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    for (BasicBlock::reverse_iterator I = BB.rbegin(), E = BB.rend(); I != E;) {
      if (isAload(&*I)) {
        CallInst *CI = dyn_cast<CallInst>(&*I);
        ++I;
        while (isAload(CI->getNextNode()) && getAloadCost(CI) < 1) {
          CI->moveAfter(CI->getNextNode());
          changed = true;
        }
      } else {
        ++I;
      }
    }
  }
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AloadReorderingPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "AloadReorderingPass") {
                    FPM.addPass(AloadReorderingPass());
                    return true;
                  }
                  return false;
                });
          }};
}