#include "passes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include <algorithm>
#include <vector>

class Cluster {
public:
  std::vector<StoreInst *> stores;
  int insts;
  Cluster() { insts = 1; }
  static std::vector<Cluster> getClusters(Module &M, int &possibleClusterNum);
};

std::vector<Cluster> Cluster::getClusters(Module &M, int &possibleClusterNum) {
  std::vector<Cluster> clusters;

  for (auto &F : M) {
    for (auto &BB : F) {
      Cluster *C = new Cluster();

      for (auto &I : BB) {
        // if the instruction is a store instruction, add it to the current
        // cluster
        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          C->stores.push_back(SI);

          C->insts++;
          if (SI->getOperand(0)->getType() != Type::getInt64Ty(M.getContext()))
            C->insts++;
          if (SI->getOperand(1)->getType() !=
              Type::getInt64PtrTy(M.getContext()))
            C->insts++;

          // if the current cluster has 7 insts, add it to clusters and clear
          // current cluster because oracle can only have up to 16 args
          if (C->stores.size() >= 7) {
            clusters.push_back(*C);
            C = new Cluster();
          }
        } else {
          // the instruction is not store (end of the current cluster)
          // if the current cluster has at least 3 instructions, add to clusters
          if (C->stores.size() >= 3)
            clusters.push_back(*C);
          C = new Cluster();
        }
      }
      // at the end of the basic block, if the current cluster has at least 3
      // instructions, add it to the clusters
      if (C->stores.size() >= 3)
        clusters.push_back(*C);
    }
  }

  // stable sort by the number of LLVM instructions in descending order
  std::stable_sort(
      clusters.begin(), clusters.end(),
      [](const Cluster &a, const Cluster &b) { return a.insts > b.insts; });

  // totalInstructions: the number of instructions in all clusters (<=48)
  int totalInstructions = 0;
  // iterate from the first element of clusters until the total number of
  // instructions is less than or equal to 48
  for (auto iter = clusters.begin(); iter != clusters.end(); iter++) {
    totalInstructions += iter->insts;
    possibleClusterNum++;
    if (totalInstructions > 48) {
      totalInstructions -= iter->insts;
      possibleClusterNum--;
      break;
    }
  }
  return clusters;
}

Function *makeOracle(Module &M, LLVMContext &CTX) {
  std::vector<Type *> params;
  // the first argument is the cluster number
  params.push_back(Type::getInt64Ty(CTX));
  // the rest of the arguments are the stored values and addresses
  for (int i = 0; i < 7; i++) {
    params.push_back(Type::getInt64Ty(CTX));
    params.push_back(Type::getInt64PtrTy(CTX));
  }
  auto *NewFT = FunctionType::get(Type::getInt64Ty(CTX), params, false);
  // generating the oracle function
  auto *NewF =
      Function::Create(NewFT, GlobalValue::ExternalLinkage, "oracle", M);
  return NewF;
}

void fillInOracle(Function *NewF, std::vector<Cluster> &clusters,
                  int possibleClusterNum, LLVMContext &CTX) {
  auto *NewBB = BasicBlock::Create(CTX, "entry", NewF);

  std::vector<BasicBlock *> BBs;
  for (int i = 0; i < possibleClusterNum; i++) {
    BBs.push_back(BasicBlock::Create(CTX, "L" + std::to_string(i + 1), NewF));
    IRBuilder<> Builder_i(BBs[i]);

    auto iter = NewF->arg_begin();
    iter++;
    for (int j = 0; j < clusters[i].stores.size(); j++) {
      Value *StoredValue = iter++;
      Value *StoredAddr = iter++;

      if (clusters[i].stores[j]->getOperand(0)->getType() !=
          Type::getInt64Ty(CTX))
        StoredValue = Builder_i.CreateTrunc(
            StoredValue, clusters[i].stores[j]->getOperand(0)->getType());
      // cast the pointer to the original type
      if (clusters[i].stores[j]->getOperand(1)->getType() !=
          Type::getInt64PtrTy(CTX))
        StoredAddr = Builder_i.CreateBitOrPointerCast(
            StoredAddr, clusters[i].stores[j]->getOperand(1)->getType());
      // create a new store instruction
      Builder_i.CreateStore(StoredValue, StoredAddr);
    }
    Builder_i.CreateRet(ConstantInt::get(Type::getInt64Ty(CTX), 0));
  }

  // end block
  auto *EndBB = BasicBlock::Create(CTX, "end", NewF);
  IRBuilder<> Builder_end(EndBB);
  Builder_end.CreateRet(ConstantInt::get(Type::getInt64Ty(CTX), 0));

  // adding the switch statement to the entry block
  IRBuilder<> Builder(NewBB);
  auto *Switch =
      Builder.CreateSwitch(NewF->arg_begin(), EndBB, possibleClusterNum);

  for (int i = 0; i < possibleClusterNum; i++) {
    Switch->addCase(ConstantInt::get(Type::getInt64Ty(CTX), i + 1), BBs[i]);
  }
}

void replaceStoreWithOracle(std::vector<Cluster> &clusters,
                            int possibleClusterNum, LLVMContext &CTX,
                            Function *NewF) {
  for (int i = 0; i < possibleClusterNum; i++) {
    IRBuilder<> Builder(clusters[i].stores[0]);
    std::vector<Value *> args;

    // the first argument is the cluster number
    args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), i + 1));

    // the next Cluster[i].size() * 2 args are the store values and addresses
    for (auto I : clusters[i].stores) {
      // cast the stored value to i64 and the stored address to i64*
      auto *StoredValue = I->getOperand(0);
      auto *StoredAddr = I->getOperand(1);

      args.push_back(Builder.CreateSExt(StoredValue, Type::getInt64Ty(CTX)));
      args.push_back(
          Builder.CreateBitOrPointerCast(StoredAddr, Type::getInt64PtrTy(CTX)));
    }
    // the rest of the arguments are 0 and null
    for (int j = clusters[i].stores.size(); j < 7; j++) {
      args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), 0));
      args.push_back(ConstantPointerNull::get(Type::getInt64PtrTy(CTX)));
    }

    Builder.CreateCall(NewF, args);
    for (auto I : clusters[i].stores) {
      I->eraseFromParent();
    }
  }
}

PreservedAnalyses OraclePass::run(Module &M, ModuleAnalysisManager &MAM) {
  for (auto &F : M) {
    if (F.getName() == "oracle")
      F.setName("not_oracle");
  }

  // possibleClusterNum: the number of clusters that can be included
  int possibleClusterNum = 0;

  auto clusters = Cluster::getClusters(M, possibleClusterNum);

  // if there are no clusters, return
  if (clusters.empty())
    return PreservedAnalyses::all();

  auto &CTX = M.getContext();

  auto *NewF = makeOracle(M, CTX);
  fillInOracle(NewF, clusters, possibleClusterNum, CTX);

  replaceStoreWithOracle(clusters, possibleClusterNum, CTX, NewF);
  return PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "OraclePass", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "OraclePass") {
                    MPM.addPass(OraclePass());
                    return true;
                  }
                  return false;
                });
          }};
}
