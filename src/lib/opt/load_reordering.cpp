#include "load_reordering.h"

/*
 *  move load to front of block
 *  load after LastDepInst -> move load after load, keep orders of load
 *
 * @LI:          load instruction to move
 * @LastDepInst: first instruction that has dependency with LI
 * return:       true if instruction moved toward
 */
bool LoadReorderingPass::moveInstruction(LoadInst *LI,
                                         Instruction *LastDepInst) {
  if (LI->getParent() == LastDepInst->getParent()) {
    BasicBlock::iterator it(LastDepInst);
    it++;
    while (dyn_cast<LoadInst>(&*it) && (&*it != LI)) {
      it++;
    }
    if (&*it == LI) {
      return false;
    }
    LI->moveAfter(LastDepInst);
    return true;
  } else {
    return false;
  }
}

/*
 * check two value have same pointer value
 * two pointers can be same if two values are loaded from same value
 *
 * @V1:    first value to compare
 * @V2:    second value to compare
 * @L:     load instruction
 * @O:     other instruction
 * return: true if two values have same value
 */

bool LoadReorderingPass::isSamePointer(Value *V1, Value *V2, LoadInst *L,
                                       Instruction *O) {
  int pointerLevel = 0;
  std::set<Value *> valueSet;
  valueSet.insert(V2);
  Value *newV1 = V1;
  Value *newV2 = V2;
  // iterate back from load instruction
  BasicBlock::iterator itStart(L);
  BasicBlock::iterator blockBegin = L->getParent()->begin();
  while (itStart != blockBegin) {
    itStart--;
    Instruction *target = &*itStart;
    if (StoreInst *SI = dyn_cast<StoreInst>(target)) {
      if (SI->getPointerOperand() == newV2) {
        pointerLevel--;
        newV2 = SI->getValueOperand();
      }
    }
    if (LoadInst *LI = dyn_cast<LoadInst>(target)) {
      if (LI == newV2) {
        pointerLevel++;
        newV2 = LI->getPointerOperand();
      }
    }
    if (!pointerLevel) {
      valueSet.insert(newV2);
    }
  }
  pointerLevel = 0;
  BasicBlock::iterator itStart2(O);
  BasicBlock::iterator blockBegin2 = O->getParent()->begin();
  while (itStart2 != blockBegin2) {
    itStart2--;
    Instruction *target = &*itStart2;
    if (StoreInst *SI = dyn_cast<StoreInst>(target)) {
      if (SI->getPointerOperand() == newV1) {
        pointerLevel--;
        newV1 = SI->getValueOperand();
      }
    }
    if (LoadInst *LI = dyn_cast<LoadInst>(target)) {
      if (LI == newV1) {
        pointerLevel++;
        newV1 = LI->getPointerOperand();
      }
    }
    if (!pointerLevel) {
      if (valueSet.count(newV1) > 0) {
        return true;
      }
    }
  }
  return false;
}

/*
 *  check dependency of load instruction L and other instruction O
 *  check the operands and users
 *
 * @L:     load instruction to check
 * @O:     other instruction to check
 * return: true if have dependency
 */
bool LoadReorderingPass::isDependent(LoadInst *L, Instruction *O) {
  for (Use &U : O->operands()) {
    if (U->getType()->isPointerTy()) {
      if (U.get() == L->getPointerOperand()) {
        return true;
      }
      if (isSamePointer(U.get(), L->getPointerOperand(), L, O)) {
        return true;
      }
    }
  }
  for (User *U : O->users()) {
    if (U->getType()->isPointerTy()) {
      if (U == L) {
        return true;
      }
    }
  }
  return false;
}

/*
 *  iterate instructions from load to first of block
 *  no dependency in block -> move to first
 *
 * @LI:    load instruction to move
 * return: true if instruction moved
 */
bool LoadReorderingPass::iterateBack(LoadInst *LI) {
  BasicBlock::iterator itStart(LI);
  BasicBlock::iterator blockBegin = LI->getParent()->begin();
  while (itStart != blockBegin) {
    --itStart;
    Instruction *target = &*itStart;
    if (isDependent(LI, target)) {
      return moveInstruction(LI, target);
    }
  }
  if (&LI->getParent()->front() == LI) {
    return false;
  }
  LI->moveBefore(&LI->getParent()->front());
  return true;
}

PreservedAnalyses LoadReorderingPass::run(Function &F,
                                          FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    for (auto &I : BB) {
      if (auto *LI = dyn_cast<LoadInst>(&I)) {
        if (iterateBack(LI))
          changed = true;
      }
    }
  }
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoadReorderingPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LoadReorderingPass") {
                    FPM.addPass(LoadReorderingPass());
                    return true;
                  }
                  return false;
                });
          }};
}