#include "passes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include <vector>

PreservedAnalyses OraclePass::run(Module &M, ModuleAnalysisManager &MAM) {
  std::vector<std::vector<StoreInst *>> Clusters;
  // TotalInstructions: the number of instructions in all clusters (must be less
  // than or equal to 48)
  // PossibleClusterNum: the number of clusters that can be included in the
  // oracle function
  int TotalInstructions = 0;
  int PossibleClusterNum = 0;
  for (auto &F : M) {
    // if the function is already named "oracle", rename it to "not_oracle"
    if (F.getName() == "oracle")
      F.setName("not_oracle");
    for (auto &BB : F) {
      std::vector<StoreInst *> Stores;
      int Insts = 1;
      for (auto &I : BB) {
        // if the instruction is a store instruction, add it to the current
        // cluster
        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          Stores.push_back(SI);
          Insts++;
          // if SI's first argument is not of type i64, add 1 to Insts
          if (SI->getOperand(0)->getType() != Type::getInt64Ty(M.getContext()))
            Insts++;
          // if SI's second argument is not of type i64*, add 1 to Insts
          if (SI->getOperand(1)->getType() !=
              Type::getInt64PtrTy(M.getContext()))
            Insts++;
          // if the current cluster has 7 instructions, add it to the clusters
          // and clear the current cluster
          // this is because the oracle function can only have up to 16
          // arguments
          if (Stores.size() >= 7) {
            auto Storescopy = Stores;
            Clusters.push_back(Storescopy);
            TotalInstructions += Insts;
            if (TotalInstructions <= 48)
              PossibleClusterNum++;
            Stores.clear();
            Insts = 1;
          }
        } else {
          // the instruction is not a store instruction (end of the current
          // cluster)
          // if the current cluster has at least 3 instructions, add it to the
          // clusters and clear the current cluster
          if (Stores.size() >= 3) {
            auto Storescopy = Stores;
            Clusters.push_back(Storescopy);
            TotalInstructions += Insts;
            if (TotalInstructions <= 48)
              PossibleClusterNum++;
          }
          Stores.clear();
          Insts = 1;
        }
      }
      if (Stores.size() >= 3) {
        // at the end of the basic block, if the current cluster has at least 3
        // instructions, add it to the clusters
        auto Storescopy = Stores;
        Clusters.push_back(Storescopy);
        TotalInstructions += Insts;
        if (TotalInstructions <= 48)
          PossibleClusterNum++;
      }
    }
  }
  // if there are no clusters, return
  if (Clusters.empty())
    return PreservedAnalyses::none();
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
    for (int j = 0; j < Clusters[i].size(); j++) {
      Value *StoredValue = iter++;
      Value *StoredAddr = iter++;
      // if the original first argument in the instruction in the cluster is not
      // i64, truncate the stored value
      if (Clusters[i][j]->getOperand(0)->getType() != Type::getInt64Ty(CTX))
        StoredValue = Builder_i.CreateTrunc(
            StoredValue, Clusters[i][j]->getOperand(0)->getType());
      // cast the pointer to the original type
      if (Clusters[i][j]->getOperand(1)->getType() != Type::getInt64PtrTy(CTX))
        StoredAddr = Builder_i.CreateBitOrPointerCast(
            StoredAddr, Clusters[i][j]->getOperand(1)->getType());
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
    IRBuilder<> Builder(Clusters[i][0]);
    std::vector<Value *> args;
    // the first argument is the cluster number
    args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), i + 1));
    // the next Cluster[i].size() * 2 arguments are the stored values and
    // addresses
    for (int j = 0; j < Clusters[i].size(); j++) {
      // cast the stored value to 64-bit integer and the stored address to
      // 64-bit pointer
      auto *StoredValue = Clusters[i][j]->getOperand(0);
      auto *StoredAddr = Clusters[i][j]->getOperand(1);
      args.push_back(Builder.CreateSExt(StoredValue, Type::getInt64Ty(CTX)));
      args.push_back(
          Builder.CreateBitOrPointerCast(StoredAddr, Type::getInt64PtrTy(CTX)));
    }
    // the rest of the arguments are 0 and null
    for (int j = Clusters[i].size(); j < 7; j++) {
      args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), 0));
      args.push_back(ConstantPointerNull::get(Type::getInt64PtrTy(CTX)));
    }
    Builder.CreateCall(NewF, args);
    for (int j = 0; j < Clusters[i].size(); j++) {
      Clusters[i][j]->eraseFromParent();
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
