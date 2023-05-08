#include "recursive_branch_condition.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"

using namespace LoopBranch;
using namespace llvm;

/*
 * Check given block contains recursion
 *
 * @BB       Basic block
 * @return   whether block contains recursion
 */
bool RecursiveBranchConditionPass::isRecursionBlock(BasicBlock &BB) {
  Function *F = BB.getParent();
  for (Instruction &I : BB) {
    CallInst *call = dyn_cast<CallInst>(&I);
    if (!call)
      continue;
    if (F == call->getCalledFunction())
      return true;
  }
  return false;
}

/*
 * Check given branch condition can be inverted
 *
 * @cond     branch condition
 * @return   whether condition can be inverted
 */
bool RecursiveBranchConditionPass::checkInvertCondition(Value *cond) {
  CmpInst *cmp = dyn_cast<CmpInst>(cond);
  return cmp &&
         cmp->getNumUses() == 1; // Compare instruction and only used once
}

/*
 * Insert condition inversion after branch and invert branch
 *
 * @ctx      LLVM Context
 * @br       target branch instruction
 */
void RecursiveBranchConditionPass::insertSelect(LLVMContext &ctx,
                                                BranchInst *br) {
  IRBuilder<> builder(br);
  Value *cond = br->getCondition();
  Value *inversion = builder.CreateSelect(
      cond, ConstantInt::getBool(IntegerType::getInt1Ty(ctx), false),
      ConstantInt::getBool(IntegerType::getInt1Ty(ctx), true));
  br->setCondition(inversion);
  br->swapSuccessors();
}

/*
 * Invert condition and branch
 *
 * @br       branch instruction
 * @cmp      compare instrucion of branch instrucion
 */
void RecursiveBranchConditionPass::invertCmp(BranchInst *br, CmpInst *cmp) {
  cmp->setPredicate(cmp->getInversePredicate());
  br->swapSuccessors();
}

/*
 * optimize recursive branch if possible
 *
 * @BB       target basic block
 * @return   changed or not
 */
bool RecursiveBranchConditionPass::optimizeBrBlock(BasicBlock &BB) {
  BranchInst *br = dyn_cast<BranchInst>(BB.getTerminator());

  if (!br || !br->isConditional())
    return false;

  BasicBlock *BBt = br->getSuccessor(0);
  BasicBlock *BBf = br->getSuccessor(1);
  Value *cond = br->getCondition();

  if (!isMoreRecursive(BBt, BBf))
    return false;

  if (checkInvertCondition(cond))
    invertCmp(br, dyn_cast<CmpInst>(cond));
  else
    insertSelect(BB.getParent()->getContext(), br);

  return true;
}

/*
 * get the number of recursion blocks that dominated by BB1
 * also find that BB1 can reaches to BB2
 *
 * @BB1      target basic block
 * @BB2      another basic block
 * @return   number of recursion and reachable
 */
RecursiveBranchConditionPass::RecursiveInfo
RecursiveBranchConditionPass::getRecursiveCnt(BasicBlock *BB1,
                                              BasicBlock *BB2) {
  SmallVector<BasicBlock *> dom;
  DT->getDescendants(BB1, dom);
  int cnt = 0;
  bool reachable = 0;
  for (BasicBlock *BB : dom) {
    if (recursionBlocks.find(BB) != recursionBlocks.end())
      cnt++;
    for (BasicBlock *BBn : successors(BB))
      reachable |= (BBn == BB2);
  }
  return {cnt, reachable};
}

/*
 * check that BBt is more recursive branch than BBf
 *
 * @BBt      true successors of branch
 * @BBf      false successors of branch
 * @return   BBt is more recursive than BBf
 */
bool RecursiveBranchConditionPass::isMoreRecursive(BasicBlock *BBt,
                                                   BasicBlock *BBf) {
  RecursiveInfo i1 = getRecursiveCnt(BBt, BBf);
  RecursiveInfo i2 = getRecursiveCnt(BBf, BBt);
  return (i1.cnt > 0 && i1.reachable) ||
         (!i1.reachable && !i2.reachable && i1.cnt > i2.cnt);
}

/*
 * check blocks in functions that contain recursion call
 * and insert to recursionBlocks
 *
 * @F        target function
 */
void RecursiveBranchConditionPass::recalculateRecursionBlocks(Function &F) {
  recursionBlocks.clear();
  for (BasicBlock &BB : F) {
    if (isRecursionBlock(BB))
      recursionBlocks.insert(&BB);
  }
}

/*
 * optimize recursion branches in funtion
 */
PreservedAnalyses
RecursiveBranchConditionPass::run(Function &F, FunctionAnalysisManager &FAM) {
  DT = &FAM.getResult<DominatorTreeAnalysis>(F);
  recalculateRecursionBlocks(F);

  bool changed = false;
  for (BasicBlock &BB : F)
    if (optimizeBrBlock(BB))
      changed = true;

  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "RecursiveBranchConditionPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "RecursiveBranchConditionPass") {
                    FPM.addPass(RecursiveBranchConditionPass());
                    return true;
                  }
                  return false;
                });
          }};
}
