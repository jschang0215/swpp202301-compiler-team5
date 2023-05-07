#include "load_to_aload.h"

/*
 * get type of value in string
 *
 * @V:     value to get type
 * return: type of value in string
 */
static std::string getTypeAsString(Value *V) {
  Type *T = V->getType();
  std::string TypeStr;
  raw_string_ostream RSO(TypeStr);
  T->print(RSO);
  return RSO.str();
}

/*
 * return cost of instruction
 *
 * @I:     instruction to get cost
 * return: cost of instruction
 */
int ToAload::LoadToAloadPass::getCost(Instruction *I) {
  auto opCode = I->getOpcode();
  if (opCode == Instruction::Add)
    return 5;
  if (opCode == Instruction::Sub)
    return 5;
  if (opCode == Instruction::Mul)
    return 1;
  if (opCode == Instruction::Load)
    return 20;
  if (opCode == Instruction::Store)
    return 20;
  if (opCode == Instruction::Ret)
    return 5; // ret -> no use of loaded vlaue -> It's okay to change to aload
  if (opCode == Instruction::Br)
    return 1;
  if (opCode == Instruction::Switch)
    return 4;
  if (I->isIntDivRem())
    return 1;
  if (I->isShift())
    return 4;
  if (I->isBitwiseLogicOp())
    return 4;
  if (opCode == Instruction::Select)
    return 1;
  if (opCode == Instruction::ICmp)
    return 1;
  if (opCode == Instruction::Call) {
    CallInst *CI = dyn_cast<CallInst>(I);
    StringRef functionName = CI->getCalledFunction()->getName();
    std::regex int_sum_pattern("int_sum_i(1|8|16|32|64)");
    std::regex aload_pattern("aload_i(8|16|32|64)");
    std::regex decr_incr_pattern("(de|in)cr_i(1|8|16|32|64)");
    std::regex assert_pattern("assert_eq_i(1|8|16|32|64)");
    if (std::regex_match(functionName.str(), int_sum_pattern))
      return 10;
    if (std::regex_match(functionName.str(), aload_pattern))
      return 1;
    if (std::regex_match(functionName.str(), decr_incr_pattern))
      return 1;
    if (std::regex_match(functionName.str(), assert_pattern))
      return 0;
    if (functionName == "oracle")
      return 40;
    if (functionName == "malloc" || functionName == "free")
      return 50;
    return CI->getCalledFunction()->arg_size() + 2;
  }
  return 0;
}

/*
 * check if cost between load and use is greater than 5
 *
 * @LI:     load instruction to check
 * return:  true if cost is greater than 5
 */
bool ToAload::LoadToAloadPass::calcCost(LoadInst *LI) {
  int cost = 0;
  Instruction *nextInst = LI->getNextNode();
  bool check = true;
  while (nextInst != nullptr && check && cost <= 5) {
    for (auto &op : nextInst->operands()) {
      if (op.get() == LI) {
        check = false;
      }
    }
    if (check)
      cost += getCost(nextInst);
    nextInst = nextInst->getNextNode();
  }
  return cost > 5;
}

/*
 * Replace load instruction to aload
 *
 * @LI:      load instruction to be replaced
 * @builder: builder of caller function
 */
void ToAload::LoadToAloadPass::loadChange(LoadInst *LI, IRBuilder<> &builder) {
  Value *ptr = LI->getPointerOperand();
  std::string funcName = "aload_" + getTypeAsString(LI);
  FunctionCallee func = LI->getFunction()->getParent()->getOrInsertFunction(
      funcName, LI->getType(), ptr->getType());
  Value *AL = builder.CreateCall(func, ptr);
  AL->setName(LI->getName());
  LI->replaceAllUsesWith(AL);
  LI->eraseFromParent();
}

PreservedAnalyses ToAload::LoadToAloadPass::run(Function &F,
                                                FunctionAnalysisManager &FAM) {
  IRBuilder<> builder(F.getContext());
  bool changed = false;
  for (auto &BB : F) {
    for (BasicBlock::reverse_iterator RI = BB.rbegin(), RE = BB.rend();
         RI != RE;) {
      Instruction &I = *RI;
      ++RI;
      if (auto *LI = dyn_cast<LoadInst>(&I)) {
        if (calcCost(LI)) {
          builder.SetInsertPoint(&I);
          loadChange(LI, builder);
          changed = true;
        }
      }
    }
  }
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoadToAloadPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LoadToAloadPass") {
                    FPM.addPass(ToAload::LoadToAloadPass());
                    return true;
                  }
                  return false;
                });
          }};
}