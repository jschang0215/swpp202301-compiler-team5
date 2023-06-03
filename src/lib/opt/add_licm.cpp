#include "add_licm.h"

PreservedAnalyses LicmPass::run(Function &F, FunctionAnalysisManager &FAM) {
  legacy::FunctionPassManager FPM(F.getParent());
  FPM.add(createLICMPass());
  FPM.run(F);

  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LicmPass", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LicmPass") {
                    FPM.addPass(LicmPass());
                    return true;
                  }
                  return false;
                });
          }};
}