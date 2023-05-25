#include "./pre_oracle.h"

/*
 * Get a set of all alloca instructions that a pointer p depends on.
 * If p depends on a pointer given as an argument, assume all such arguments are
 * dependent.
 *
 * @p:     pointer value
 * @return set of all allocas that p depends on, including null if p depends on
 * an argument
 */
std::set<AllocaInst *> getAllocas(Value &p) {
  std::set<AllocaInst *> allocas;

  // if p is not pointer, return empty set
  if (!p.getType()->isPointerTy())
    return allocas;

  if (auto *Arg = dyn_cast<Argument>(&p)) {
    // if p is an argument, we don't know what it points to
    // so we assume they are all the same pointer
    if (Arg->getType()->isPointerTy())
      allocas.insert(nullptr);
  } else if (auto *AI = dyn_cast<AllocaInst>(&p)) {
    allocas.insert(AI);
  } else if (auto *I = dyn_cast<Instruction>(&p)) {
    // recursively find all allocas that p depends on
    for (auto &op : I->operands()) {
      std::set<AllocaInst *> tmp = getAllocas(*op.get());
      allocas.insert(tmp.begin(), tmp.end());
    }
  }

  return allocas;
}

/*
 * Check if two pointers p1 and p2 may be dependent.
 *
 * @p1:     pointer value
 * @p2:     pointer value
 * @return  true if p1 and p2 may be dependent, false otherwise
 */
bool checkPointerDependency(Value &p1, Value &p2) {
  // if either p1 or p2 is not pointer, return false
  if (!p1.getType()->isPointerTy() || !p2.getType()->isPointerTy())
    return false;

  // if p1 and p2 are same, return true
  if (&p1 == &p2)
    return true;

  // if p1 and p2 are same global variable, return true
  if (auto *GV1 = dyn_cast<GlobalVariable>(&p1))
    if (auto *GV2 = dyn_cast<GlobalVariable>(&p2))
      if (GV1 == GV2)
        return true;

  auto allocas1 = getAllocas(p1);
  auto allocas2 = getAllocas(p2);
  // if two sets have common elements, return true
  for (auto *AI : allocas1)
    if (allocas2.find(AI) != allocas2.end())
      return true;

  return false;
}

/*
 * Reorder instructions in stores so that instructions that can move past I are
 * clustered at the end.
 *
 * @stores: vector of stores to reorder
 * @I:      load or call instruction to check dependency against
 * @return  number of insts starting from the beginning that can't move past I
 */
int reorderStores(std::vector<StoreInst *> &stores, Instruction &I) {
  // similar to bubble sort
  int last = stores.size();
  for (int i = last - 1; i >= 0; i--) {
    bool isIndependent = true;
    for (auto &op : I.operands()) {
      if (checkPointerDependency(*op.get(), *stores[i]->getPointerOperand())) {
        isIndependent = false;
        break;
      }
    }
    if (!isIndependent)
      continue;
    int j = i;
    for (; j < last - 1; j++) {
      auto *p1 = stores[j]->getPointerOperand();
      auto *p2 = stores[j + 1]->getPointerOperand();
      if (!checkPointerDependency(*p1, *p2)) {
        auto *temp = stores[j];
        stores[j] = stores[j + 1];
        stores[j + 1] = temp;
      } else
        break;
    }
    if (j == last - 1)
      last--;
  }
  return last;
}

PreservedAnalyses PreOraclePass::run(Function &F,
                                     FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    // stores: list of encountered stores that have not been clustered
    std::vector<StoreInst *> stores;

    for (auto &I : BB) {
      if (auto *SI = dyn_cast<StoreInst>(&I)) {
        stores.push_back(SI);
      } else if (isa<LoadInst>(I) || isa<CallInst>(I) || I.isTerminator()) {
        if (stores.empty())
          continue;

        if (I.isTerminator()) {
          // if I is terminator, this is the end of the BB
          if (stores.empty())
            break;
          // deposit all remaining instructions in stores before terminator
          for (auto *SI : stores)
            SI->moveBefore(&I);
          changed = true;
          break;
        }

        // otherwise, I is a load or call instruction
        int last = reorderStores(stores, I);
        // if all insts in stores are independent of I, they can all move past I
        if (!last)
          continue;
        // otherwise, if keeping 3 stores clustered before I is possible, do it
        if (last < 3 && stores.size() >= 3)
          last = 3;
        // deposit insts that can't move past I before I
        for (int i = 0; i < last; i++)
          stores[i]->moveBefore(&I);
        changed = true;
        // remove deposited insts from stores
        stores.erase(stores.begin(), stores.begin() + last);
      }
    }
  }
  if (!changed)
    return PreservedAnalyses::all();
  return PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PreOraclePass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "PreOraclePass") {
                    FPM.addPass(PreOraclePass());
                    return true;
                  }
                  return false;
                });
          }};
}