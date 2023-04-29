#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <map>
#include <queue>

using namespace llvm;

class SimplePass : public PassInfoMixin<SimplePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

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
  void replaceToSum(Value *curNode, std::vector<Value *> &leafNodes, Function &F);
  void eliminateDeadAdd(Function &F);
};