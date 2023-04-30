#include "passes.h"

/* 
*  move load to front of block
*  load after LastDepInst -> move load after load : keep orders of load
*/
static bool moveInstruction(LoadInst *LI, Instruction *LastDepInst){
  if(LI->getParent() == LastDepInst->getParent()){
    while(dyn_cast<LoadInst>(LastDepInst->getNextNode()) && (LastDepInst->getNextNode()!=LI)){
      LastDepInst = LastDepInst->getNextNode();
    }
    if(LastDepInst->getNextNode()==LI){
      return false;
    }
    LI->moveAfter(LastDepInst);
    return true;
  }else{
    return false;
  }
}

/*
*  check dependency of load instruction L and other instruction O
*  chek the operands and users
*/
static bool isDependent(LoadInst *L, Instruction *O){
  for(Use &U : O->operands()){
    if(U->getType()->isPointerTy()){
      if(U.get() == L->getPointerOperand()){
        return true;
      }
    }
  }
  for(User *U : O->users()){
    if(U->getType()->isPointerTy()){
      if(U == L){
        return true;
      }
    }
  }
  return false;
}

/*
*  iterate instructions from load to first of block
*  no dependency in block -> move to first
*/
static bool iterateBack(LoadInst *LI){
  BasicBlock::iterator itStart(LI);
  BasicBlock::iterator blockBegin = LI->getParent()->begin();
  while(itStart != blockBegin){
    --itStart;
    Instruction *target = &*itStart;
    if(isDependent(LI, target)) {
      return moveInstruction(LI, target);
    }
  }
  if(&LI->getParent()->front()==LI){
    return false;
  }
  LI->moveBefore(&LI->getParent()->front());
  return true;
}

PreservedAnalyses LoadReorderingPass::run(Function &F, FunctionAnalysisManager &FAM){
  bool changed = false;
    for(auto &BB : F){
        for(auto &I : BB){
          if(auto *LI = dyn_cast<LoadInst>(&I)){
            if(iterateBack(LI)) changed = true;
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