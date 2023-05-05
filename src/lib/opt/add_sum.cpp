#include "add_sum.h"

/*
 * Helper function that returns type of Value *V
 *
 * @V:      value to get type as string
 * return:  return type as string (e.x. "i32")
 */
static std::string getTypeAsString(Value *V) {
  Type *T = V->getType();
  std::string TypeStr;
  raw_string_ostream RSO(TypeStr);
  T->print(RSO);
  return RSO.str();
}

void AddSumPass::getLeafNodes(AddNode *node, std::vector<Value *> &leafNodes) {
  if (node->Children.empty()) {
    leafNodes.push_back(node->value);
  } else {
    for (auto &child : node->Children) {
      getLeafNodes(child, leafNodes);
    }
  }
}

/*
 * Get nodes in addDependencyTree that are used in other than add instruction.
 * These nodes are refered as 'useful' nodes.
 *
 * @usefulNodes:          result to store 'useful' nodes
 * @addDependencyTree:    addDepedencyTree
 * @F:                    function to be looked up
 */
void AddSumPass::getUsefulNodes(std::set<Value *> &usefulNodes,
                                std::map<Value *, AddNode *> &addDependencyTree,
                                Function &F) {
  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      for (unsigned i = 0, e = I.getNumOperands(); i != e; ++i) {
        Value *operand = I.getOperand(i);

        /* Check if operand is part of addDependencyTree */
        if (addDependencyTree.find(operand) != addDependencyTree.end()) {
          /*Check if the operand is used in an instruction other than Add */
          if (I.getOpcode() != Instruction::Add) {
            usefulNodes.insert(operand);
          }
        }
      }
    }
  }
}

/*
 * Replace to sum instruction.
 * Replace value to sum instruction, by summing leaf node of tree that has
 * 'value' as root in addDependencyTree.
 * Sum instruction has 8 arguments.
 * Case 1, 2 is splitted when leafNodes has under/above 8 entries.
 *
 * @value:          node that will be replaced to sum instruction
 * @leafNodes:      leafNodes of value node
 * @F:              used for replacing instruction
 */
void AddSumPass::replaceToSum(Value *value, std::vector<Value *> &leafNodes,
                              Function &F) {
  /* sum instruction template */
  std::string funcName = "int_sum_" + getTypeAsString(value);
  FunctionCallee sumFunc = F.getParent()->getOrInsertFunction(
      funcName,
      FunctionType::get(value->getType(),
                        std::vector<Type *>(8, value->getType()), false));

  if (leafNodes.size() <= 8) {
    /*
     * Case 1. If leafNodes size <= 8, replace I with
     * call @int_sum(leafNodes[0], ..., leafNodes[size of leafNodes])
     * if leafNodes size < 8, pad with 0
     *  ex. leafNodes size is 3, then, replace I with
     *  call @int_sum(leafNodes[0], leafNodes[1], leafNodes[2], 0, 0, 0, 0, 0)
     */

    /* First, pad with zeros */
    while (leafNodes.size() < 8)
      leafNodes.push_back(ConstantInt::get(value->getType(), 0));

    Instruction *oldInst = dyn_cast<Instruction>(value);
    CallInst *newInst =
        CallInst::Create(sumFunc, ArrayRef<Value *>(leafNodes), "", oldInst);

    /* Replace all uses of the old instruction with the new instruction */
    oldInst->replaceAllUsesWith(newInst);
    oldInst->eraseFromParent();
  } else {
    /*
     * Case 2. If leafNodes size > 8, cascade @int_sum
     *  Case 2-1. After cascading and there is only 1 operand left at last,
     *    cascade @int_sum, and at last, use add instruction
     *    ex. leafNodes size is 9, replace with
     *    %0 = call @int_sum([0], ..., [7]), %1 = add %0, [8]
     *  Case 2-2. Else, cascade @int_sum
     *    ex. leafNodes size is 10, replace with
     *    %0 = call @int_sum([0], ..., [7]), %1 = call @int_sum(%0, [8], [9])
     */

    /* First, fill sum instruction much as possible */
    size_t lastIndex = (leafNodes.size() - 8) / 7 * 7 + 8;
    Value *sum = nullptr;
    std::vector<Value *> chunk;
    for (size_t i = 0; i < lastIndex;) {
      if (i == 0) {
        /* When first using sum instruction, fill all with leafNodes */
        chunk = std::vector<Value *>(leafNodes.begin() + i,
                                     leafNodes.begin() + i + 8);
        i += 8;
      } else {
        /* Next, start with previous sum instruction result and fill with
         * leafNodes */
        chunk = std::vector<Value *>(leafNodes.begin() + i,
                                     leafNodes.begin() + i + 7);
        chunk.insert(chunk.begin(), sum);
        i += 7;
      }

      Instruction *insertBefore = dyn_cast<Instruction>(value);
      CallInst *newInst = CallInst::Create(sumFunc, chunk, "", insertBefore);
      sum = newInst;
    }

    if (leafNodes.size() - lastIndex == 1) {
      /* Case 2-1 */
      Instruction *oldInst = dyn_cast<Instruction>(value);
      oldInst->setOperand(0, sum);
      oldInst->setOperand(1, leafNodes.back());
    } else {
      /* Case 2-2 */
      chunk = std::vector<Value *>(leafNodes.begin() + lastIndex,
                                   leafNodes.begin() + leafNodes.size());
      chunk.insert(chunk.begin(), sum);

      while (chunk.size() < 8)
        chunk.push_back(ConstantInt::get(value->getType(), 0));

      Instruction *insertBefore = dyn_cast<Instruction>(value);
      CallInst *newInst = CallInst::Create(sumFunc, chunk, "", insertBefore);
      Instruction *oldInst = dyn_cast<Instruction>(value);

      oldInst->replaceAllUsesWith(newInst);
      oldInst->eraseFromParent();
    }
  }
}

/*
 * Eliminate unneccesary add instructions.
 * This results from conversion to sum instruction.
 * This implementation uses -adce optimization.
 */
void AddSumPass::eliminateDeadAdd(Function &F) {
  legacy::FunctionPassManager FPM(F.getParent());

  /* ADCE pass manager */
  PassManagerBuilder PMB;
  PMB.populateFunctionPassManager(FPM);

  /*
   * Repeat -adce optimization untill there is no change in code
   * -adce does not optimize fully when calling only once.
   */
  int prevInstCount, curInstCount;
  do {
    prevInstCount = std::distance(inst_begin(F), inst_end(F));
    FPM.run(F);
    curInstCount = std::distance(inst_begin(F), inst_end(F));
  } while (prevInstCount != curInstCount);
}

/*
 * Contrust add dependecy Tree in basic block.
 * Result is stored in @addDependencyTree.
 */
void AddSumPass::createAddDependencyTree(BasicBlock &BB, std::map<Value *, AddNode *> &addDependencyTree) {
  for (Instruction &I : BB) {
    if (I.getOpcode() == Instruction::Add) {
      Value *Result = &I;
      addDependencyTree[Result] = new AddNode{Result};

      for (int i = 0; i < 2; i++) {
        Value *operand = I.getOperand(i);

        if (!addDependencyTree.count(operand)) {
          addDependencyTree[operand] = new AddNode{operand};
        }

        addDependencyTree[Result]->Children.push_back(
            addDependencyTree[operand]);
      }
    }
  }
}

PreservedAnalyses AddSumPass::run(Function &F, FunctionAnalysisManager &FAM) {
  bool changed = false;
  SmallVector<Instruction *, 8> toErase;

  /*
   * Construct add instruction dependecy tree.
   * Operand for each add instruction's result is appended to Children.
   */
  for (BasicBlock &BB : F) {
    std::map<Value *, AddNode *> addDependencyTree;

    createAddDependencyTree(BB, addDependencyTree);

    /* Nodes that are also used besides add instruction */
    std::set<Value *> usefulNodes;
    getUsefulNodes(usefulNodes, addDependencyTree, F);

    for (auto &node : usefulNodes) {
      std::vector<Value *> leafNodes;
      getLeafNodes(addDependencyTree[node], leafNodes);

      /* Apply optimization only when there is more than 2 operands */
      if (leafNodes.size() <= 2)
        continue;

      replaceToSum(node, leafNodes, F);
      changed = true;
    }

    /* Garbage collection */
    for (auto &node : addDependencyTree)
      delete node.second;
  }

  eliminateDeadAdd(F);

  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AddSumPass", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "AddSumPass") {
                    FPM.addPass(AddSumPass());
                    return true;
                  }
                  return false;
                });
          }};
}