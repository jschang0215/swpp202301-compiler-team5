#include "passes.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Instruction.h"

static std::string getTypeAsString(Value *V){
    Type *T = V->getType();
    std::string TypeStr;
    raw_string_ostream RSO(TypeStr);
    T->print(RSO);
    return RSO.str();
}

static int getCost(Instruction *I){
    if(I->getOpcode() == Instruction::Add) return 5;
    if(I->getOpcode() == Instruction::Sub) return 5;
    if(I->getOpcode() == Instruction::Mul) return 1;
    if(I->isIntDivRem()) return 1;
    if(I->isShift()) return 4;
    if(I->isBitwiseLogicOp()) return 4;
    if(I->getOpcode() == Instruction::Select) return 1;
    if(I->getOpcode() == Instruction::ICmp) return 1;
    if(I->getOpcode() == Instruction::Call){
        CallInst* CI = dyn_cast<CallInst>(I);
        StringRef functionName = CI->getCalledFunction()->getName();
        if(functionName == "aload_i8" || functionName == "aload_i16" || functionName == "aload_i32" || functionName == "aload_i64") return 1;
        if(functionName == "int_sum_i1" || functionName == "int_sum_i8" || functionName == "int_sum_i16" || functionName == "int_sum_i32" || functionName == "int_sum_i64") return 10;
        if(functionName == "incr_i1" || functionName == "incr_i8" || functionName == "incr_i16" || functionName == "incr_i32" || functionName == "incr_i64") return 1;
        if(functionName == "decr_i1" || functionName == "decr_i8" || functionName == "decr_i16" || functionName == "decr_i32" || functionName == "decr_i64") return 1;
        if(functionName == "assert_eq_i1" || functionName == "assert_eq_i8" || functionName == "assert_eq_i16" || functionName == "assert_eq_i32" || functionName == "assert_eq_i64") return 0;
        if(functionName == "oracle") return 40;
        if(functionName == "malloc" || functionName == "free") return 50;
        return CI->getCalledFunction()->arg_size() + 2;
    }
    if(I->getOpcode() == Instruction::Load) return 20;
    if(I->getOpcode() == Instruction::Store) return 20;
    if(I->getOpcode() == Instruction::Ret) return 5; //ret -> no use of loaded vlaue -> It's okay to change to aload
    if(I->getOpcode() == Instruction::Br) return 1;
    if(I->getOpcode() == Instruction::Switch) return 4;
    return 0;
}

PreservedAnalyses LoadToAloadPass::run(Function &F, FunctionAnalysisManager &FAM){
  IRBuilder<> builder(F.getContext());
  SmallVector<Instruction*, 8> toErase;
  bool changed = false;

    for(auto &BB : F){
        for(BasicBlock::reverse_iterator RI = BB.rbegin(), RE = BB.rend(); RI != RE; ++RI){
            Instruction &I = *RI;
            if(auto *LI = dyn_cast<LoadInst>(&I)){
                int cost = 0;
                Instruction *nextInst = LI->getNextNode();
                bool check = true;
                while(nextInst != nullptr && check){
                    for(auto &op : nextInst->operands()){
                        if(op.get() == LI){
                            check = false;
                        }
                    }
                    if(!is_contained(toErase, nextInst) && check) cost += getCost(nextInst);
                    nextInst = nextInst->getNextNode();
                }
                if(cost > 5){
                    builder.SetInsertPoint(&I);
                    Value *ptr = LI->getPointerOperand();
                    std::string funcName = "aload_" + getTypeAsString(LI);
                    FunctionCallee func = LI->getFunction()->getParent()->getOrInsertFunction(funcName, LI->getType(), ptr->getType());
                    Value *AL = builder.CreateCall(func, ptr);
                    AL->setName(LI->getName());
                    LI->replaceAllUsesWith(AL);
                    toErase.push_back(LI);
                }
            }
        }
    }
    for(auto *I : toErase) {
        I->eraseFromParent();
        changed = true;
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
                    FPM.addPass(LoadToAloadPass());
                    return true;
                  }
                  return false;
                });
          }};
}