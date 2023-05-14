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

bool LoadReorderingPass::isSamePointer(Value *V1, Value *V2, Instruction *L,
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
  if (auto *CI = dyn_cast<CallInst>(O))
    return true;
  for (Use &U : O->operands()) {
    if (U.get() == L->getPointerOperand()) {
      return true;
    }
    if (isSamePointer(U.get(), L->getPointerOperand(), L, O)) {
      return true;
    }
  }
  for (User *U : O->users()) {
    if (U == L) {
      return true;
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
  if (LI->getParent()->getFirstNonPHI() == LI) {
    return false;
  }
  Instruction *first = LI->getParent()->getFirstNonPHI();
  if (LI->getParent() == first->getParent()) {
    BasicBlock::iterator it(first);
    while (dyn_cast<LoadInst>(&*it) && (&*it != LI)) {
      it++;
    }
    if (&*it == LI) {
      return false;
    }
    LI->moveBefore(first);
    return true;
  } else {
    return false;
  }
}

/*
 * check dependency between two instructions
 * memory access instruction and call instruction : check always true
 *
 * @I1:    former instruction to check
 * @I2:    latter instruction to check
 * return: true if there is dependency
 */

bool LoadReorderingPass::dependencyCheck(Instruction *I1, Instruction *I2) {
  // inside call, memory could be accessed
  if (auto *C = dyn_cast<CallInst>(I2)) {
    if (I1->mayReadOrWriteMemory()) {
      return true;
    }
    if (auto *C1 = dyn_cast<CallInst>(I1)) {
      return true;
    }
  }
  if (auto *C = dyn_cast<CallInst>(I1)) {
    if (I2->mayReadOrWriteMemory()) {
      return true;
    }
  }
  // check store instruction
  if (auto *S = dyn_cast<StoreInst>(I2)) {
    Value *p = S->getPointerOperand();
    for (Use &Op : I1->operands()) {
      if (Op.get() == p)
        return true;
      if (isSamePointer(Op.get(), p, I2, I1)) {
        return true;
      }
    }
  }
  if (auto *S = dyn_cast<StoreInst>(I1)) {
    Value *p = S->getPointerOperand();
    for (Use &Op : I2->operands()) {
      if (Op.get() == p)
        return true;
      if (isSamePointer(p, Op.get(), I2, I1)) {
        return true;
      }
    }
  }
  // check dependency
  for (Use &Op : I2->operands()) {
    if (Op.get() == I1) {
      return true;
    }
  }
  return false;
}

/*
 * move use instruction of load backward
 * move I to just before first dependent instruction of I
 *
 * @I:      instruction to move back
 * return:  true if instruction moved
 */
bool LoadReorderingPass::moveBack(Instruction *I) {
  // terminator, call instruction don't move
  if (auto *CI = dyn_cast<CallInst>(I))
    return false;
  if (I->isTerminator())
    return false;
  BasicBlock *BB = I->getParent();
  int count = 0;
  BasicBlock::iterator it(I);
  BasicBlock::iterator end = BB->end();
  it++;
  while (it != end) {
    count++;
    if (dependencyCheck(I, &*it)) {
      I->moveBefore(&*it);
      break;
    }
    if (it->isTerminator()) {
      I->moveBefore(&*it);
      break;
    }
    it++;
  }
  if (count > 1) {
    return true;
  } else {
    return false;
  }
}

/*
 * move instruction between load and use
 *
 * @I:     instruction that has no dependency with load and use
 * @FI:    first use instruction of load
 * return: true if instruction moved
 */
bool LoadReorderingPass::moveForward(Instruction *I, Instruction *FI) {
  BasicBlock::iterator it(I);
  BasicBlock::iterator begin = I->getParent()->begin();
  --it;
  while (it != begin) {
    if (dependencyCheck(&*it, I))
      return false;
    if (&*it == FI) {
      I->moveBefore(FI);
      return true;
    }
    --it;
  }
  return true;
}

PreservedAnalyses LoadReorderingPass::run(Function &F,
                                          FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    // load instruction reordering
    for (auto &I : BB) {
      if (auto *LI = dyn_cast<LoadInst>(&I)) {
        if (iterateBack(LI))
          changed = true;
      }
    }
    // use instruction reordering
    std::set<Instruction *> uses;
    for (auto &I : BB) {
      if (auto *LI = dyn_cast<LoadInst>(&I)) {
        bool check = false;
        // find use instructions and move back
        BasicBlock::iterator it(LI);
        BasicBlock::iterator end = BB.end();
        ++it;
        while (it != end) {
          for (auto &op : it->operands()) {
            if (op.get() == LI) {
              check = true;
            }
          }
          if (check) {
            uses.insert(&*it);
            check = false;
          }
          ++it;
        }
      }
    }
    // avoid dependecy betweene use instructions - reorder instructions in
    // reverse order
    for (auto u = uses.rbegin(); u != uses.rend(); ++u) {
      if (moveBack(*u))
        changed = true;
    }
    // other instruction reordering
    for (auto &I : BB) {
      if (auto *LI = dyn_cast<LoadInst>(&I)) {
        bool check = false;
        BasicBlock::iterator it(LI);
        BasicBlock::iterator end = BB.end();
        ++it;
        Instruction *FI = NULL;
        while (it != end) {
          if (it->isTerminator())
            break;
          if (check && uses.count(&*it) == 0) {
            if (moveForward(&*it, FI))
              changed = true;
          } else {
            for (auto &op : it->operands()) {
              if (op.get() == LI) {
                check = true;
                FI = &*it;
              }
            }
          }
          ++it;
        }
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