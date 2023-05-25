#ifndef SWPP_PRE_ORACLE
#define SWPP_PRE_ORACLE

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include <vector>
#include <set>

using namespace llvm;

class PreOraclePass : public PassInfoMixin<PreOraclePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

#endif
