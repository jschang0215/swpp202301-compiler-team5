#include "./pre_oracle.h"

PreservedAnalyses PreOraclePass::run(Function &F,
                                     FunctionAnalysisManager &FAM) {
  bool changed = false;
  for (auto &BB : F) {
    // stores: list of encountered stores that have not been clustered
    std::vector<StoreInst *> stores;
    for (auto &I : BB) {
      if (auto *SI = dyn_cast<StoreInst>(&I)) {
        stores.push_back(SI);
      } else if (isa<LoadInst>(I) || isa<CallInst>(I) || I.isTerminator()) {
        // load and call instructions can have unexpected dependencies, so the
        // naive approach cannot move the store instructions past them
        if (stores.size() < 3) {
          // if less than three stores, no need to cluster
          stores.clear();
          continue;
        }
        // move all instructions in stores to just before I
        changed = true;
        for (auto *SI : stores)
          SI->moveBefore(&I);
        stores.clear();
      }
    }
  }
  if (!changed)
    return PreservedAnalyses::all();
  return PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PreOraclePass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "PreOraclePass") {
                    FPM.addPass(PreOraclePass());
                    return true;
                  }
                  return false;
                });
          }};
}