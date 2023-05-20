#ifndef SWPP_ORACLE
#define SWPP_ORACLE

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include <algorithm>
#include <vector>

using namespace llvm;

class Cluster {
public:
  std::vector<StoreInst *> stores;
  int insts;
  int in_loop;
  Cluster() { insts = 1; in_loop = 0; }
  Cluster(int in_loop): in_loop(in_loop) { insts = 1; }
  static std::vector<Cluster> processClusters(std::vector<Cluster> clusters);
  static std::vector<Cluster> getClusters(Module &M, ModuleAnalysisManager &MAM);
};

class OraclePass : public PassInfoMixin<OraclePass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

#endif
