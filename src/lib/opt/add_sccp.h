#ifndef SWPP_SCCP
#define SWPP_SCCP

#include "llvm/Pass.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/Transforms/Scalar/SCCP.h"

using namespace llvm;

class SccpPass : public PassInfoMixin<SccpPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

};

#endif