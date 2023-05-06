#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class SimplePass : public PassInfoMixin<SimplePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

class ShiftConstantAddPass : public PassInfoMixin<ShiftConstantAddPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

private:
  void shiftToOp(Instruction *I, Instruction::BinaryOps Operator,
                  IRBuilder<> &Builder);
};

class OraclePass : public PassInfoMixin<OraclePass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};