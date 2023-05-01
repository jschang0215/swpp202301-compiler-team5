#include "passes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include <algorithm>
#include <vector>

class Cluster {
public:
  std::vector<StoreInst *> Stores;
  int Insts;
  Cluster() { Insts = 1; }
};

PreservedAnalyses OraclePass::run(Module &M, ModuleAnalysisManager &MAM) {
  std::vector<Cluster> Clusters;
  for (auto &F : M) {
    // if the function is already named "oracle", rename it to "not_oracle"
    if (F.getName() == "oracle")
      F.setName("not_oracle");
    for (auto &BB : F) {
      Cluster *C = new Cluster();
      for (auto &I : BB) {
        // if the instruction is a store instruction, add it to the current
        // cluster
        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          C->Stores.push_back(SI);
          C->Insts++;
          // if SI's first argument is not of type i64, add 1 to Insts
          if (SI->getOperand(0)->getType() != Type::getInt64Ty(M.getContext()))
            C->Insts++;
          // if SI's second argument is not of type i64*, add 1 to Insts
          if (SI->getOperand(1)->getType() !=
              Type::getInt64PtrTy(M.getContext()))
            C->Insts++;
          // if the current cluster has 7 instructions, add it to the clusters
          // and clear the current cluster
          // this is because the oracle function can only have up to 16
          // arguments
          if (C->Stores.size() >= 7) {
            Clusters.push_back(*C);
            C = new Cluster();
          }
        } else {
          // the instruction is not a store instruction (end of the current
          // cluster)
          // if the current cluster has at least 3 instructions, add it to the
          // clusters and clear the current cluster
          if (C->Stores.size() >= 3)
            Clusters.push_back(*C);
          C = new Cluster();
        }
      }
      // at the end of the basic block, if the current cluster has at least 3
      // instructions, add it to the clusters
      if (C->Stores.size() >= 3)
        Clusters.push_back(*C);
    }
  }
  // if there are no clusters, return
  if (Clusters.empty())
    return PreservedAnalyses::none();
  // stable sort the clusters by the number of LLVM instructions in descending
  // order
  std::stable_sort(
      Clusters.begin(), Clusters.end(),
      [](const Cluster &a, const Cluster &b) { return a.Insts > b.Insts; });
  // TotalInstructions: the number of instructions in all clusters (must be less
  // than or equal to 48)
  // PossibleClusterNum: the number of clusters that can be included in the
  // oracle function
  int TotalInstructions = 0;
  int PossibleClusterNum = 0;
  // iterate from the first element of Clusters until the total number of
  // instructions is less than or equal to 48
  for (auto iter = Clusters.begin(); iter != Clusters.end(); iter++) {
    TotalInstructions += iter->Insts;
    PossibleClusterNum++;
    if (TotalInstructions > 48) {
      TotalInstructions -= iter->Insts;
      PossibleClusterNum--;
      break;
    }
  }
  // generating the oracle function type
  auto &CTX = M.getContext();
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
  // entry block
  auto *NewBB = BasicBlock::Create(CTX, "entry", NewF);
  // generating the basic blocks for each cluster
  std::vector<BasicBlock *> BBs;
  for (int i = 0; i < PossibleClusterNum; i++) {
    BBs.push_back(BasicBlock::Create(CTX, "L" + std::to_string(i + 1), NewF));
    IRBuilder<> Builder_i(BBs[i]);
    auto iter = NewF->arg_begin();
    iter++;
    for (int j = 0; j < Clusters[i].Stores.size(); j++) {
      Value *StoredValue = iter++;
      Value *StoredAddr = iter++;
      // if the original first argument in the instruction in the cluster is not
      // i64, truncate the stored value
      if (Clusters[i].Stores[j]->getOperand(0)->getType() !=
          Type::getInt64Ty(CTX))
        StoredValue = Builder_i.CreateTrunc(
            StoredValue, Clusters[i].Stores[j]->getOperand(0)->getType());
      // cast the pointer to the original type
      if (Clusters[i].Stores[j]->getOperand(1)->getType() !=
          Type::getInt64PtrTy(CTX))
        StoredAddr = Builder_i.CreateBitOrPointerCast(
            StoredAddr, Clusters[i].Stores[j]->getOperand(1)->getType());
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
      Builder.CreateSwitch(NewF->arg_begin(), EndBB, PossibleClusterNum);
  for (int i = 0; i < PossibleClusterNum; i++) {
    Switch->addCase(ConstantInt::get(Type::getInt64Ty(CTX), i + 1), BBs[i]);
  }
  // replacing the store instructions with the oracle function call
  for (int i = 0; i < PossibleClusterNum; i++) {
    IRBuilder<> Builder(Clusters[i].Stores[0]);
    std::vector<Value *> args;
    // the first argument is the cluster number
    args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), i + 1));
    // the next Cluster[i].size() * 2 arguments are the stored values and
    // addresses
    for (auto I : Clusters[i].Stores) {
      // cast the stored value to 64-bit integer and the stored address to
      // 64-bit pointer
      auto *StoredValue = I->getOperand(0);
      auto *StoredAddr = I->getOperand(1);
      args.push_back(Builder.CreateSExt(StoredValue, Type::getInt64Ty(CTX)));
      args.push_back(
          Builder.CreateBitOrPointerCast(StoredAddr, Type::getInt64PtrTy(CTX)));
    }
    // the rest of the arguments are 0 and null
    for (int j = Clusters[i].Stores.size(); j < 7; j++) {
      args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), 0));
      args.push_back(ConstantPointerNull::get(Type::getInt64PtrTy(CTX)));
    }
    Builder.CreateCall(NewF, args);
    for (auto I : Clusters[i].Stores) {
      I->eraseFromParent();
    }
  }
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
