#include "passes.h"

PreservedAnalyses SimplePass::run(Function &F, FunctionAnalysisManager &FAM) {
    return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SimplePass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "SimplePass") {
                    FPM.addPass(SimplePass());
                    return true;
                  }
                  return false;
                });
          }};
}
