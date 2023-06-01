#include "add_o3.h"

PreservedAnalyses O3Pass::run(Function &F, FunctionAnalysisManager &FAM) {
  legacy::FunctionPassManager FPM(F.getParent());

  PassManagerBuilder PMB;
  PMB.OptLevel = 3;
  PMB.populateFunctionPassManager(FPM);

  int prevInstCount, curInstCount;
  do {
    prevInstCount = std::distance(inst_begin(F), inst_end(F));
    FPM.run(F);
    curInstCount = std::distance(inst_begin(F), inst_end(F));
  } while (prevInstCount != curInstCount);

  return PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "O3Pass", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "O3Pass") {
                    FPM.addPass(O3Pass());
                    return true;
                  }
                  return false;
                });
          }};
}