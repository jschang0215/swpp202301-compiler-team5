#include "passes.h"

/*
 * Helper function that returns type of Value *V
 *
 * @V:      value to get type as string
 * return:  return type as string (e.x. "i32")
 */
static std::string getTypeAsString(Value *V) {
  Type *T = V->getType();
  std::string TypeStr;
  raw_string_ostream RSO(TypeStr);
  T->print(RSO);
  return RSO.str();
}

/*
 * Replace shift instruction with given Operator.
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

/*
 * Replace constant add/sub instruction.
 * Only replace if -5 < constVal < 5.
 * Replace to call incr/decr
 *
 * @I:          instruction to be replaced
 * @constVal:   operand of constant value
 * @Builder:    builder of caller function
 * return:      true when replaced
 */
static bool constantAddSub(Instruction *I, ConstantInt *constVal,
                           IRBuilder<> &Builder) {
  /* Get constVal value */
  int64_t constValSExt = constVal->getSExtValue();

  /* Cehck is instruction is add or sub */
  bool isAdd = false, isSub = false;
  if (auto *add = dyn_cast<AddOperator>(I))
    isAdd = true;
  if (auto *sub = dyn_cast<SubOperator>(I))
    isSub = true;

  /*
   * Optimization can be only applied when -5 < constVal < 5
   * since incr/decr cost is 1, add/sub cost is 5
   */
  if (constValSExt > -5 && constValSExt < 5) {
    Value *val = I->getOperand(0);
    /*
     * Case1. add & constVal > 0: convert to incr
     * Case2. sub & constVal < 0: convert to incr
     */
    if ((isAdd && constValSExt >= 0) || (isSub && constValSExt <= 0)) {
      /*
       * Replace with incr instruction.
       * IR intrinsic is named as "incr_" + type of val.
       * Param type of incr is set as type of val
       */
      std::string funcName = "incr_" + getTypeAsString(val);
      llvm::FunctionCallee func =
          I->getFunction()->getParent()->getOrInsertFunction(
              funcName, val->getType(), val->getType());
      for (int i = 0; i < std::abs(constValSExt); i++)
        val = Builder.CreateCall(func, {val});
      I->replaceAllUsesWith(val);
      return true;
    }
    /*
     * Case3. add & constVal < 0: convert to decr
     * Case4. sub & constVal > 0: convert to decr
     */
    else if ((isAdd && constValSExt < 0) || (isSub && constValSExt > 0)) {
      /* Same as incr, except IR intrinsic is "decr_" + type of val. */
      std::string funcName = "decr_" + getTypeAsString(val);
      llvm::FunctionCallee func =
          I->getFunction()->getParent()->getOrInsertFunction(
              funcName, val->getType(), val->getType());
      for (int i = 0; i < std::abs(constValSExt); i++)
        val = Builder.CreateCall(func, {val});
      I->replaceAllUsesWith(val);
      return true;
    }
  }
  return false;
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

      /* Shift instruction optimization */
      if (auto *shl = dyn_cast<ShlOperator>(&I)) {
        /* left shift is replaced to mul */
        shiftToOp(&I, Instruction::Mul, Builder);
        toErase.push_back(&I);
        changed = true;
      } else if (auto *lshr = dyn_cast<LShrOperator>(&I)) {
        /* Logical right shift is replaced to unsigned div */
        shiftToOp(&I, Instruction::UDiv, Builder);
        toErase.push_back(&I);
        changed = true;
      } else if (auto *ashr = dyn_cast<AShrOperator>(&I)) {
        /*
         * Arithmetic right shift is replacable only when operand is guaranteed
         * to be positive. i.e. operand is positive constant Replace ashr only
         * when operand is constant positive.
         */
        if (ConstantInt *constVal = dyn_cast<ConstantInt>(I.getOperand(0))) {
          if (!constVal->isNegative()) {
            shiftToOp(&I, Instruction::SDiv, Builder);
            toErase.push_back(&I);
            changed = true;
          }
        }
      }

      /* Constant add, sub instruction optimization */
      if (auto *addSub = dyn_cast<BinaryOperator>(&I)) {
        if (auto *constVal = dyn_cast<ConstantInt>(addSub->getOperand(1))) {
          if (constantAddSub(addSub, constVal, Builder)) {
            /* Erase original instruction when substituted */
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