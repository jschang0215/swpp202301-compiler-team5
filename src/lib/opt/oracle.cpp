#include "passes.h"
#include "llvm/IR/Constants.h"
#include <vector>

PreservedAnalyses OraclePass::run(Module &M, ModuleAnalysisManager &MAM) {
  std::vector<std::vector<Instruction *>> Clusters;
  int TotalInstructions = 0;
  int PossibleClusterNum = 0;
  for (auto &F : M) {
    if (F.getName() == "oracle")
      F.setName("not_oracle");
    for (auto &BB : F) {
      std::vector<Instruction *> Stores;
      for (auto &I : BB) {
        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          Stores.push_back(SI);
          if (Stores.size() >= 7) {
            auto Storescopy = Stores;
            Clusters.push_back(Storescopy);
            TotalInstructions += Stores.size();
            if (TotalInstructions < 50)
              PossibleClusterNum++;
            Stores.clear();
          }
        } else {
          if (Stores.size() >= 3) {
            auto Storescopy = Stores;
            Clusters.push_back(Storescopy);
            TotalInstructions += Stores.size();
            if (TotalInstructions < 50)
              PossibleClusterNum++;
          }
          Stores.clear();
        }
      }
      if (Stores.size() >= 3) {
        auto Storescopy = Stores;
        Clusters.push_back(Storescopy);
        TotalInstructions += Stores.size();
        if (TotalInstructions < 50)
          PossibleClusterNum++;
      }
    }
  }
  if (Clusters.empty())
    return PreservedAnalyses::all();
  auto &CTX = M.getContext();
  std::vector<Type *> params;
  params.push_back(Type::getInt64Ty(CTX));
  for (int i = 0; i < 7; i++) {
    params.push_back(Type::getInt64Ty(CTX));
    params.push_back(Type::getInt64PtrTy(CTX));
  }
  auto *NewFT = FunctionType::get(Type::getInt64Ty(CTX), params, false);
  auto *NewF =
      Function::Create(NewFT, GlobalValue::ExternalLinkage, "oracle", M);
  auto *NewBB = BasicBlock::Create(CTX, "entry", NewF);
  std::vector<BasicBlock *> BBs;
  for (int i = 0; i < PossibleClusterNum; i++) {
    BBs.push_back(BasicBlock::Create(CTX, "L" + std::to_string(i + 1), NewF));
    IRBuilder<> Builder_i(BBs[i]);
    auto iter = NewF->arg_begin();
    iter++;
    for (int j = 0; j < Clusters[i].size(); j++) {
      auto *StoredValue = iter++;
      auto *StoredAddr = iter++;
      Builder_i.CreateStore(StoredValue, StoredAddr);
    }
    Builder_i.CreateRet(ConstantInt::get(Type::getInt64Ty(CTX), 0));
  }
  auto *EndBB = BasicBlock::Create(CTX, "end", NewF);
  IRBuilder<> Builder_end(EndBB);
  Builder_end.CreateRet(ConstantInt::get(Type::getInt64Ty(CTX), 0));
  IRBuilder<> Builder(NewBB);
  auto *Switch =
      Builder.CreateSwitch(NewF->arg_begin(), EndBB, PossibleClusterNum);
  for (int i = 0; i < PossibleClusterNum; i++) {
    Switch->addCase(ConstantInt::get(Type::getInt64Ty(CTX), i + 1), BBs[i]);
  }
  for (int i = 0; i < PossibleClusterNum; i++) {
    IRBuilder<> Builder(Clusters[i][0]);
    std::vector<Value *> args;
    args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), i + 1));
    for (int j = 0; j < Clusters[i].size(); j++) {
      auto *StoredValue = Clusters[i][j]->getOperand(0);
      auto *StoredAddr = Clusters[i][j]->getOperand(1);
      args.push_back(Builder.CreateSExt(StoredValue, Type::getInt64Ty(CTX)));
      args.push_back(Builder.CreateBitOrPointerCast(StoredAddr, Type::getInt64PtrTy(CTX)));
    }
    for (int j = Clusters[i].size(); j < 7; j++) {
      args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), 0));
      args.push_back(ConstantPointerNull::get(Type::getInt64PtrTy(CTX)));
    }
    Builder.CreateCall(NewF, args);
    for (int j = 0; j < Clusters[i].size(); j++) {
      Clusters[i][j]->eraseFromParent();
    }
  }
  return PreservedAnalyses::all();
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
