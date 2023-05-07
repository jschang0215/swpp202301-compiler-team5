#ifndef SWPP_ADD_SUM
#define SWPP_ADD_SUM

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <iostream>
#include <map>
#include <queue>

using namespace llvm;

class AddSumPass : public PassInfoMixin<AddSumPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

private:
  struct AddNode {
    Value *value;
    std::vector<AddNode *> Children;
  };

  void getLeafNodes(AddNode *node, std::vector<Value *> &leafNodes);
  void getUsefulNodes(std::set<Value *> &usefulNodes,
                      std::map<Value *, AddNode *> &addDependencyTree,
                      Function &F);
  void replaceToSum(Value *curNode, std::vector<Value *> &leafNodes,
                    Function &F);
  void eliminateDeadAdd(Function &F);
  void createAddDependencyTree(BasicBlock &BB, std::map<Value *, AddNode *> &addDependencyTree);
  unsigned int defaultCost(AddNode *node, std::map<Value *, AddNode *> &addDependencyTree, std::set<AddNode *> history);
  unsigned int optimizedCost(std::vector<Value *> leafNodes);
};

#endif