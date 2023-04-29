#include "passes.h"

/*
 * Replace instruction with given Operator.
 * Given instruction is expected to have replacable shift instructions.
 * Operator is expected to be mul, udiv, sdiv.
 * 
 * @I:          instruction to be replaced
 * @Operator:   operator to be substituted
 * @Builder:    builder of caller function
 */
static void shiftToOp(Instruction *I, Instruction::BinaryOps Operator,
                      IRBuilder<> &Builder) {
  Value *val = I->getOperand(0);
  Value *n = I->getOperand(1);

  /* New operand with 1 << n to be replaced */
  Constant *newOperand = ConstantInt::get(
      val->getType(), 1 << cast<ConstantInt>(n)->getZExtValue());

  Value *newOperator = Builder.CreateBinOp(Operator, val, newOperand);
  I->replaceAllUsesWith(newOperator);
}

PreservedAnalyses ShiftConstantAddPass::run(Function &F,
                                            FunctionAnalysisManager &FAM) {
  bool changed = false;
  IRBuilder<> Builder(F.getContext());

  /* 
   * Instruction to be removed after applying shiftToOp
   * All instruction applied shiftToOp can be erased.
   */
  SmallVector<Instruction *, 8> toErase;

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      Builder.SetInsertPoint(&I);
      if (auto *Shl = dyn_cast<ShlOperator>(&I)) {
        /* left shift is replaced to mul */
        shiftToOp(&I, Instruction::Mul, Builder);
        toErase.push_back(&I);
        changed = true;
      } else if (auto *LShr = dyn_cast<LShrOperator>(&I)) {
        /* Logical right shift is replaced to unsigned div */
        shiftToOp(&I, Instruction::UDiv, Builder);
        toErase.push_back(&I);
        changed = true;
      } else if (auto *AShr = dyn_cast<AShrOperator>(&I)) {
        /*
         * Arithmetic right shift is replacable only when operand is guaranteed to be positive.
         * i.e. operand is positive constant
         * Replace ashr only when operand is constant positive.
         */
        if (ConstantInt *constVal = dyn_cast<ConstantInt>(I.getOperand(0))) {
          if (!constVal->isNegative()) {
            shiftToOp(&I, Instruction::SDiv, Builder);
            toErase.push_back(&I);
            changed = true;
          }
        }
      }
    }
  }

  /* Erase instructions */
  for (auto *I : toErase)
    I->eraseFromParent();

  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ShiftConstantAddPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "ShiftConstantAddPass") {
                    FPM.addPass(ShiftConstantAddPass());
                    return true;
                  }
                  return false;
                });
          }};
}