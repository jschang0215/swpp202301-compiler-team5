#include "heap_promotion.h"

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
 * Check if a Value is used as a function argument except for free
 *
 * @Var:      value to check
 * return:    true if the value is used as a function argument
 */
static bool isUsedAsFuncArg(Value *Var) {
  for (User *U : Var->users()) {
    if (auto *CI = dyn_cast<CallInst>(U)) {
      Function *Callee = CI->getCalledFunction();
      if (Callee && Callee->getName() == "free") {
        continue;
      }

      for (unsigned i = 0, e = CI->getNumOperands(); i != e; ++i) {
        if (CI->getOperand(i) == Var) {
          return true;
        }
      }
    }
  }
  return false;
}

/*
 * Find a matching free call from of related variables.
 * Assume collectrlatedVar is called for malloced variable.
 *
 * @relatedVars: relatedVars
 * return:       found free CallInst*, nullptr if not found
 */
static CallInst *findMatchingFreeCall(std::vector<Value *> &relatedVars) {
  for (Value *Var : relatedVars) {
    if (auto *CI = dyn_cast<CallInst>(Var)) {
      Function *Callee = CI->getCalledFunction();
      if (Callee && Callee->getName() == "free") {
        return CI;
      }
    }
  }
  return nullptr;
}

PreservedAnalyses HeapPromotionPass::run(Function &F,
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

  outs() << "Function: " << F.getName() << "\n";

  for (CallInst *MallocCall : mallocCalls) {
    // Check if the malloc argument is a constant
    Value *MallocArg = MallocCall->getArgOperand(0);
    if (!isa<ConstantInt>(MallocArg)) {
      continue;
    }

    // Collect all related variables
    std::vector<Value *> relatedVar;
    collectrelatedVar(MallocCall, relatedVar);

    // Find matching free call
    CallInst *matchedFreeCall = findMatchingFreeCall(relatedVar);

    // If no corresponding free call, skip optimiz1ation
    if (!matchedFreeCall)
      continue;

    // Check if the allocated memory or any related variable is used as a
    // function call argument
    bool usedAsFuncArg = false;
    for (Value *Var : relatedVar) {
      if (isUsedAsFuncArg(Var)) {
        usedAsFuncArg = true;
        break;
      }
    }

    // Skip if used as function call argument
    if (usedAsFuncArg)
      continue;

    outs() << "Found: " << *MallocCall << "\n";
    for (Value *Var : relatedVar) {
      outs() << "   Related: " << *Var << "\n";
    }

    /*
    Change to alloca
    */
  }

  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HeapPromotionPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "HeapPromotionPass") {
                    FPM.addPass(HeapPromotionPass());
                    return true;
                  }
                  return false;
                });
          }};
}