#ifndef SWPP_LOAD_REORDERING
#define SWPP_LOAD_REORDERING

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class LoadReorderingPass : public PassInfoMixin<LoadReorderingPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

private:
  bool iterateBack(LoadInst *LI);
  bool isDependent(LoadInst *L, Instruction *O);
  bool isSamePointer(Value *V1, Value *V2, LoadInst *L, Instruction *O);
  bool moveInstruction(LoadInst *LI, Instruction *LastDepInst);
};

#endif