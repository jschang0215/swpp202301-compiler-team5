#include "malloc_free_reordering.h"

/*
 * Collect related variables of V recursively.
 * Get all var associated with the malloced variable.
 * Use as @collectrelatedVar(MALLOC_VAR, relatedVars)
 *
 * @V:            value to start collecting related variables
 * @relatedVars:  related var result
 */
static void collectrelatedVar(Value *V, std::vector<Value *> &relatedVars) {
  for (auto &v : relatedVars) {
    if (v == V)
      return;
  }
  relatedVars.push_back(V);

  for (User *U : V->users()) {
    if (auto *storeInst = dyn_cast<StoreInst>(U)) {
      Value *storedValue = storeInst->getOperand(1);
      collectrelatedVar(storedValue, relatedVars);
    } else if (isa<Instruction>(U) || isa<Argument>(U)) {
      collectrelatedVar(cast<Value>(U), relatedVars);
    }
  }
}

/*
 * Collect all instructions with relatedVars
 *
 * @F:            function to be looked at
 * @relatedVars:  related var
 * @relatedInsts:  result
 */
static void collectrelatedInst(Function &F, std::vector<Value *> &relatedVars,
                               std::vector<Instruction *> &relatedInsts) {
  // Iterate through all the instructions in the function
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    Instruction *Inst = &*I;

    // Check each operand of the instruction
    for (unsigned i = 0, e = Inst->getNumOperands(); i != e; ++i) {
      Value *operand = Inst->getOperand(i);

      // If the operand is one of the relatedVars, add the instruction to
      // relatedInsts
      if (std::find(relatedVars.begin(), relatedVars.end(), operand) !=
          relatedVars.end()) {
        relatedInsts.push_back(Inst);
        break;
      }
    }
  }
}

/*
 * Get free instruction from found related instructions
 *
 * @relatedInsts:  Related nstruction to be searched on
 */
static CallInst *getfreeInst(std::vector<Instruction *> &relatedInsts) {
  CallInst *freeInst = nullptr;

  for (auto &I : relatedInsts) {
    if (auto *CI = dyn_cast<CallInst>(I)) {
      Function *Callee = CI->getCalledFunction();
      if (Callee && Callee->getName() == "free") {
        // return null if multiple free detected
        if (!freeInst)
          freeInst = CI;
        else
          return nullptr;
      }
    }
  }

  return freeInst;
}

/*
 * Collect all instruction consecutive to malloc.
 * No other instruction (outside from relatedInsts) is in middle.
 * Group should be in same basic block.
 *
 * @V:              malloc inst
 * @relatedInsts:   vector to be searched
 * @mallocGroups:   result
 */
static void collectmallocGroup(Value *V,
                               std::vector<Instruction *> &relatedInsts,
                               std::vector<Instruction *> &mallocGroups) {
  Instruction *curTop = dyn_cast<Instruction>(V);

  for (auto &I : relatedInsts) {
    // return if not in same basic block
    if (I->getParent() != curTop->getParent())
      break;

    if (I == curTop->getNextNode()) {
      mallocGroups.push_back(I);
      curTop = I;
    } else {
      break;
    }
  }
}

/*
 * Collect all instruction backward consecutive to free.
 * No other instruction (outside from relatedInsts) is in middle.
 * Group should be in same basic block.
 *
 * @V:              free inst
 * @relatedInsts:   vector to be searched
 * @freeGroups:   result
 */
static void collectfreeGroup(Value *V, std::vector<Instruction *> &relatedInsts,
                             std::vector<Instruction *> &freeGroups) {
  Instruction *curBottom = dyn_cast<Instruction>(V);

  for (auto it = relatedInsts.rbegin(); it != relatedInsts.rend(); ++it) {
    Instruction *I = *it;

    // Skip for free instruction
    if (I == V)
      continue;

    // return if not in same basic block
    if (I->getParent() != curBottom->getParent())
      break;

    if (I == curBottom->getPrevNode()) {
      freeGroups.insert(freeGroups.begin(), I);
      curBottom = I;
    } else {
      break;
    }
  }
}

/*
 * Find first use after mallocGroup.
 *
 * @relatedInsts:   related instructions
 * @mallocGroups:   malloc groups to get last use in malloc group
 */
static Instruction *findFirstUse(std::vector<Instruction *> &relatedInsts,
                                 std::vector<Instruction *> &mallocGroups) {
  Instruction *lastMallocGroup = mallocGroups.back();

  for (auto it = relatedInsts.begin(); it != relatedInsts.end(); ++it) {
    if (*it == lastMallocGroup)
      return (++it != relatedInsts.end()) ? *it : nullptr;
  }

  return nullptr;
}

/*
 * Find last use before freeGroups.
 *
 * @relatedInsts:   related instructions
 * @freeGroups:     free groups to get first use in free group
 */
static Instruction *findLastUse(std::vector<Instruction *> &relatedInsts,
                                std::vector<Instruction *> &freeGroups) {
  Instruction *fristFreeGroup = freeGroups.front();

  for (auto it = relatedInsts.rbegin(); it != relatedInsts.rend(); ++it) {
    if (*it == fristFreeGroup)
      return (++it != relatedInsts.rend()) ? *it : nullptr;
  }

  return nullptr;
}

PreservedAnalyses MallocFreeReorderingPass::run(Function &F,
                                                FunctionAnalysisManager &FAM) {
  bool changed = false;

  std::vector<CallInst *> mallocCalls;

  // Find malloc and free calls
  for (auto &BB : F) {
    for (auto &I : BB) {
      if (auto *CI = dyn_cast<CallInst>(&I)) {
        Function *Callee = CI->getCalledFunction();
        if (Callee && Callee->getName() == "malloc")
          mallocCalls.push_back(CI);
      }
    }
  }

  for (CallInst *MallocCall : mallocCalls) {

    // Collect all related variables
    std::vector<Value *> relatedVar;
    collectrelatedVar(MallocCall, relatedVar);

    // Collect all instructions with relatedVar
    std::vector<Instruction *> relatedInst;
    collectrelatedInst(F, relatedVar, relatedInst);

    CallInst *FreeCall = getfreeInst(relatedInst);
    // no free or multiple frees is not supported
    if (!FreeCall)
      continue;

    /*
    mallocGroup: Consecutive instruction just after malloc
    Instructions in mallocGroup can be moved all together.
    */
    std::vector<Instruction *> mallocGroup;
    mallocGroup.push_back(MallocCall);
    collectmallocGroup(MallocCall, relatedInst, mallocGroup);

    /*
    firstUse after mallocGroup
    i.e. there is other instructions between mallocGroup and firstUse
    */
    Instruction *firstUse = findFirstUse(relatedInst, mallocGroup);

    /*
    freeGroup: Consecutive instruction just before free
    Instructions in freeGroup can be moved all together.
    */
    std::vector<Instruction *> freeGroup;
    freeGroup.push_back(FreeCall);
    collectfreeGroup(FreeCall, relatedInst, freeGroup);

    // lastUse before mallocGroup
    Instruction *lastUse = findLastUse(relatedInst, freeGroup);

    if (firstUse) {
      for (Instruction *Inst : mallocGroup) {
        // Do not move if they are not in same basic block
        if (Inst->getParent() != firstUse->getParent())
          break;

        Inst->removeFromParent();
        Inst->insertBefore(firstUse);
      }
    }

    if (lastUse) {
      for (Instruction *Inst : freeGroup) {
        // Do not move if they are not in same basic block
        if (Inst->getParent() != lastUse->getParent())
          break;

        Inst->removeFromParent();
        Inst->insertAfter(lastUse);
        lastUse = Inst;
      }
    }
  }

  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MallocFreeReorderingPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "MallocFreeReorderingPass") {
                    FPM.addPass(MallocFreeReorderingPass());
                    return true;
                  }
                  return false;
                });
          }};
}