#include "oracle.h"

/*
 * Get all the clusters of store instructions in a module
 *
 * @M:    module to be looked at
 * @MAM:  module analysis manager
 *
 * @return vector of Cluster objects
 */
std::vector<Cluster> Cluster::getClusters(Module &M,
                                          ModuleAnalysisManager &MAM) {
  std::vector<Cluster> clusters;

  for (auto &F : M) {
    // if function is declaration and not definition, skip
    if (F.isDeclaration() || F.isIntrinsic())
      continue;

    // get loop analysis
    DominatorTree DT;
    DT.recalculate(F);
    auto loopInfo = new LoopInfoBase<BasicBlock, Loop>();
    loopInfo->releaseMemory();
    loopInfo->analyze(DT);

    for (auto &BB : F) {
      int in_loop = 0;
      for (auto l : *loopInfo) {
        if (l->contains(&BB))
          in_loop++;
      }

      Cluster *C = new Cluster(in_loop);

      for (auto &I : BB) {
        // if the instruction is store, add it to the current cluster
        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          C->stores.push_back(SI);

          C->insts++;
          if (SI->getOperand(0)->getType() != Type::getInt64Ty(M.getContext()))
            C->insts++;
          if (SI->getOperand(1)->getType() !=
              Type::getInt64PtrTy(M.getContext()))
            C->insts++;

          /* if the current cluster has 7 insts, add it to clusters and clear
           current cluster because oracle can only have up to 16 args */
          if (C->stores.size() >= 7) {
            clusters.push_back(*C);
            delete C;
            C = new Cluster(in_loop);
          }
        } else {
          /* the instruction is not store (end of the current cluster)
           if the current cluster has at least 3 instructions, add to clusters
         */
          if (C->stores.size() >= 3)
            clusters.push_back(*C);
          if (!C->stores.empty()) {
            delete C;
            C = new Cluster(in_loop);
          }
        }
      }
      /* at the end of the basic block, if the current cluster has at least 3
       instructions, add it to the clusters */
      if (C->stores.size() >= 3)
        clusters.push_back(*C);
      delete C;
    }

    delete loopInfo;
  }

  return clusters;
}

/*
 * Sort the clusters by priority, and cut off at 48 llvm instructions
 *
 * @clusters:   vector of clusters to be processed
 *
 * @return processed vector of Cluster objects
 */
std::vector<Cluster> Cluster::processClusters(std::vector<Cluster> clusters) {
  std::stable_sort(clusters.begin(), clusters.end(),
                   [](const Cluster &a, const Cluster &b) {
                     if (a.in_loop == b.in_loop)
                       return (a.stores.size() - 2) * b.insts >
                              (b.stores.size() - 2) * a.insts;
                     return a.in_loop > b.in_loop;
                   });

  std::vector<Cluster> ret;
  // totalInstructions: the number of instructions in all clusters (<=48)
  int totalInstructions = 0;
  /* iterate from the first element of clusters until the total number of
   instructions is less than or equal to 48 */
  for (auto iter = clusters.begin(); iter != clusters.end(); iter++) {
    totalInstructions += iter->insts;
    ret.push_back(*iter);
    if (totalInstructions > 48) {
      ret.pop_back();
      break;
    }
  }
  return ret;
}

/*
 * Create a new oracle function
 *
 * @M:    module to be looked at
 * @CTX:  context of the module
 *
 * @return a pointer to the new oracle function
 */
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

/*
 * Fill in the oracle function with the switch statement and new basic blocks
 *
 * @NewF:     pointer to the new oracle function
 * @clusters: vector of clusters
 * @CTX:      context of the module
 */
void fillInOracle(Function *NewF, std::vector<Cluster> &clusters,
                  LLVMContext &CTX) {
  auto *NewBB = BasicBlock::Create(CTX, "entry", NewF);

  std::vector<BasicBlock *> BBs;
  for (int i = 0; i < clusters.size(); i++) {
    BBs.push_back(BasicBlock::Create(CTX, "L" + std::to_string(i + 1), NewF));
    IRBuilder<> Builder_i(BBs[i]);

    auto iter = NewF->arg_begin();
    iter++;
    for (int j = 0; j < clusters[i].stores.size(); j++) {
      Value *StoredValue = iter++;
      Value *StoredAddr = iter++;

      if (clusters[i].stores[j]->getOperand(0)->getType() !=
          Type::getInt64Ty(CTX)) {
        if (clusters[i].stores[j]->getOperand(0)->getType()->isPointerTy())
          StoredValue = Builder_i.CreateBitOrPointerCast(
              StoredValue, clusters[i].stores[j]->getOperand(0)->getType());
        else
          StoredValue = Builder_i.CreateSExtOrTrunc(
              StoredValue, clusters[i].stores[j]->getOperand(0)->getType());
      }
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
      Builder.CreateSwitch(NewF->arg_begin(), EndBB, clusters.size());

  for (int i = 0; i < clusters.size(); i++) {
    Switch->addCase(ConstantInt::get(Type::getInt64Ty(CTX), i + 1), BBs[i]);
  }
}

/*
 * Replace the original store instructions with oracle calls
 *
 * @clusters: vector of clusters
 * @CTX:      context of the module
 * @NewF:     pointer to the new oracle function
 */
void replaceStoreWithOracle(std::vector<Cluster> &clusters, LLVMContext &CTX,
                            Function *NewF) {
  for (int i = 0; i < clusters.size(); i++) {
    IRBuilder<> Builder(clusters[i].stores[0]);
    std::vector<Value *> args;

    // the first argument is the cluster number
    args.push_back(ConstantInt::get(Type::getInt64Ty(CTX), i + 1));

    // the next Cluster[i].size() * 2 args are the store values and addresses
    for (auto I : clusters[i].stores) {
      // cast the stored value to i64 and the stored address to i64*
      auto *StoredValue = I->getOperand(0);
      auto *StoredAddr = I->getOperand(1);

      if (StoredValue->getType()->isPointerTy())
        args.push_back(
            Builder.CreatePtrToInt(StoredValue, Type::getInt64Ty(CTX)));
      else
        args.push_back(Builder.CreateSExtOrTrunc(StoredValue, Type::getInt64Ty(CTX)));
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
  auto clusters = Cluster::getClusters(M, MAM);
  // if there are no clusters, return
  if (clusters.empty())
    return PreservedAnalyses::all();

  clusters = Cluster::processClusters(clusters);

  for (auto &F : M) {
    if (F.getName() == "oracle")
      F.setName("not_oracle");
  }

  auto &CTX = M.getContext();

  auto *NewF = makeOracle(M, CTX);
  fillInOracle(NewF, clusters, CTX);

  replaceStoreWithOracle(clusters, CTX, NewF);
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
