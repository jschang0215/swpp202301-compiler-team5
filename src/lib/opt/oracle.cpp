#include "passes.h"

PreservedAnalyses OraclePass::run(Function &F, FunctionAnalysisManager &FAM) {
    return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "OraclePass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "oracle") {
                    FPM.addPass(OraclePass());
                    return true;
                  }
                  return false;
                });
          }};
}
