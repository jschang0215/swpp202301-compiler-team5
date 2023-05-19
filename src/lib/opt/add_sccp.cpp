#include "add_sccp.h"

PreservedAnalyses SccpPass::run(Function &F, FunctionAnalysisManager &FAM) {
    SCCPPass Sccp;
    return Sccp.run(F, FAM);
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SccpPass", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "SccpPass") {
                    FPM.addPass(SccpPass());
                    return true;
                  }
                  return false;
                });
          }};
}