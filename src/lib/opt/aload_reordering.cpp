#include "aload_reordering.h"
#include "load_to_aload.h"

/*
 * check instruction is aload
 *
 * @I:     instruction to check
 * return: true if I is aload
 */
bool AloadReorderingPass::isAload(Instruction *I) {
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
bool AloadReorderingPass::isMalloc(Instruction *I) {
  if (CallInst *CI = dyn_cast<CallInst>(I)) {
    StringRef functionName = CI->getCalledFunction()->getName();
    if (functionName == "malloc")
      return true;
  }
  return false;
}

/*
 * check aload ptr is in heap or stack
 *
 * @CI:    ptr to check
 * return: true if stack false if heap
 */
bool AloadReorderingPass::checkStackHeap(Value *ptr) {
  int pointerLevel = 0;
  Value *newPtr = ptr;
  Instruction *I = dyn_cast<Instruction>(ptr);
  BasicBlock::iterator itStart(I);
  BasicBlock::iterator blockBegin = I->getParent()->begin();
  itStart--;
  while (itStart != blockBegin) {
    if (LoadInst *LI = dyn_cast<LoadInst>(&*itStart)) {
      if (LI == newPtr) {
        pointerLevel++;
        newPtr = LI->getPointerOperand();
      }
    } else if (StoreInst *SI = dyn_cast<StoreInst>(&*itStart)) {
      if (SI->getPointerOperand()) {
        pointerLevel--;
        newPtr = SI->getValueOperand();
        if (!pointerLevel) {
          if (CallInst *CI = dyn_cast<CallInst>(&*itStart)) {
            if (isMalloc(CI))
              return false;
          } else if (AllocaInst *AI = dyn_cast<AllocaInst>(&*itStart)) {
            return true;
          }
        }
      }
    }
    itStart--;
  }
  return false;
}

/*
 * calculate cost of aload instruction
 *
 * @CI:    aload instruction to calculate
 * return: cost
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
  Value *ptr = CI->getArgOperand(0);
  if (AllocaInst *I = dyn_cast<AllocaInst>(ptr)) {
    return 24 - cost > 0 ? 24 - cost : 1;
  } else if (CallInst *I = dyn_cast<CallInst>(ptr)) {
    if (isMalloc(I)) {
      return 34 - cost > 0 ? 34 - cost : 1;
    } else {
      if (checkStackHeap(ptr)) {
        return 24 - cost > 0 ? 24 - cost : 1;
      } else {
        return 34 - cost > 0 ? 34 - cost : 1;
      }
    }
  } else {
    if (checkStackHeap(ptr)) {
      return 24 - cost > 0 ? 24 - cost : 1;
    } else {
      return 34 - cost > 0 ? 34 - cost : 1;
    }
  }
}

PreservedAnalyses AloadReorderingPass::run(Function &F,
                                           FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    for (BasicBlock::reverse_iterator I = BB.rbegin(), E = BB.rend(); I != E;) {
      if (isAload(&*I)) {
        CallInst *CI = dyn_cast<CallInst>(&*I);
        ++I;
        while (isAload(CI->getNextNode()) && getAloadCost(CI) == 1) {
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