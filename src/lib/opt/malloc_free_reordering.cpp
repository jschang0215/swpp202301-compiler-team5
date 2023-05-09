#include "malloc_free_reordering.h"

PreservedAnalyses MallocFreeReorderingPass::run(Function &F,
                                                FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    /*
    heapVarMap stores info of malloc & bitcast instruction.
    Goal is to move mallc & bitcast to just before firstuse.
    */
    std::map<Value *, HeapVarInfo> heapVarMap;

    findMalloc(BB, heapVarMap);
    changed |= moveMalloc(heapVarMap);
  }
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

/*
Get is bitcast(or store) instruction of malloc.
Usually malloc is used as ex. int *p = malloc(8).
This code is compiled as 2 instructions: call malloc & bitcast
The bitcast instruction should also be moved with malloc.

@I:       instruction to be checked wheter it is bitcast operation of malloc
@target:  malloced variable
return:   bitcasdt instruction (nullptr if not found)
*/
static Value *getBitCastOperand(Instruction *I, Value *target) {
  Value *res = nullptr;
  bool targetFound = false;

  for (auto &Op : I->operands()) {
    if (Op == target)
      targetFound = true;
    else
      res = Op;
  }
  if (!targetFound)
    return nullptr;

  if (res == nullptr)
    res = I;

  return res;
}

/*
Find malloc in basicblock.

@BB:          basic block where search is done
@heapVarMap:  heap var info datastructure
*/
void MallocFreeReorderingPass::findMalloc(
    BasicBlock &BB, std::map<Value *, HeapVarInfo> &heapVarMap) {
  for (auto &I : BB) {
    if (auto *CI = dyn_cast<CallInst>(&I)) {
      Function *Callee = CI->getCalledFunction();
      if (Callee && Callee->getName() == "malloc") {
        Instruction *bitCastMallocInst =
            dyn_cast<Instruction>(getBitCastOperand(CI->getNextNode(), CI));
        heapVarMap[CI] = HeapVarInfo{CI, bitCastMallocInst, nullptr};
      }
    }
  }

  for (auto &I : BB) {
    updateFirstUse(I, heapVarMap);
  }
}

/*
Update first use of malloced variable

@I:           instruction to be checked if it has malloced variable
@heapVarMap:  heap var info data structure
*/
void MallocFreeReorderingPass::updateFirstUse(
    Instruction &I, std::map<Value *, HeapVarInfo> &heapVarMap) {
  for (auto &HeapVar : heapVarMap) {
    if (!HeapVar.second.FirstUse) {
      for (auto &Op : I.operands()) {
        if (Op == HeapVar.second.BitCastMallocInst &&
            &I != HeapVar.second.MallocInst &&
            &I != HeapVar.second.MallocInst->getNextNode()) {
          HeapVar.second.FirstUse = &I;
          break;
        }
      }
    }
  }
}

/*
Move malloc & bitcast just before first use.
*/
bool MallocFreeReorderingPass::moveMalloc(
    std::map<Value *, HeapVarInfo> &heapVarMap) {
  bool modified = false;

  for (auto &HeapVar : heapVarMap) {
    Instruction *MallocInst = HeapVar.second.MallocInst;
    Instruction *BitCastMallocInst = HeapVar.second.BitCastMallocInst;
    Instruction *FirstUse = HeapVar.second.FirstUse;

    if (MallocInst && FirstUse) {
      Instruction *MallocStore = MallocInst->getNextNode();

      // Move the malloc just before the first use
      MallocInst->removeFromParent();
      MallocInst->insertBefore(FirstUse);

      // Move the malloc's store instruction just after the malloc
      MallocStore->removeFromParent();
      MallocStore->insertAfter(MallocInst);

      modified = true;
    }
  }
  return modified;
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