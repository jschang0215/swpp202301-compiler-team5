#include "branch_likely_analysis.h"
#include "loop_analysis.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/PassManager.h"

using namespace BranchLikely;

/*
 * Check given block contains recursion
 *
 * @BB       Basic block
 * @return   whether block contains recursion
 */
bool RecursiveBrInfo::isRecursionBlock(BasicBlock &BB) {
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
 * check blocks in functions that contain recursion call
 * and insert to recursionBlocks
 *
 * @F        target function
 */
void RecursiveBrInfo::recalculateRecursionBlocks(Function &F) {
  recursionBlocks.clear();
  for (BasicBlock &BB : F) {
    if (isRecursionBlock(BB))
      recursionBlocks.insert(&BB);
  }
}

/*
 * get the number of recursion blocks that dominated by BB1
 * also find that BB1 can reaches to BB2
 *
 * @BB1      target basic block
 * @BB2      another basic block
 * @return   number of recursion and reachable
 */
RecursiveBrInfo::RecursiveInfo
RecursiveBrInfo::getRecursiveCnt(BasicBlock *BB1, BasicBlock *BB2) {
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
bool RecursiveBrInfo::isMoreRecursive(BasicBlock *BBt, BasicBlock *BBf) {
  RecursiveInfo i1 = getRecursiveCnt(BBt, BBf);
  RecursiveInfo i2 = getRecursiveCnt(BBf, BBt);
  return (i1.cnt > 0 && i1.reachable) ||
         (!i1.reachable && !i2.reachable && i1.cnt > i2.cnt);
}

/*
 * add br to brSet if recursive
 *
 * @BB       target basic block
 */
void RecursiveBrInfo::addRecursiveBrLikely(BasicBlock &BB) {
  BranchInst *br = dyn_cast<BranchInst>(BB.getTerminator());

  if (!br || !br->isConditional())
    return;

  BasicBlock *BBt = br->getSuccessor(0);
  BasicBlock *BBf = br->getSuccessor(1);
  Value *cond = br->getCondition();

  if (isMoreRecursive(BBt, BBf))
    brSet.insert({br, true});
  else if (isMoreRecursive(BBf, BBt))
    brSet.insert({br, false});
}

/*
 * Getter of brSet
 */
BrSet RecursiveBrInfo::getBrSet() { return brSet; }

/*
 * Recalculate recursive branches in given funcion
 *
 * @F       target function
 * @FAM     function analysis manager
 */
void RecursiveBrInfo::recalculate(Function &F, FunctionAnalysisManager &FAM) {
  brSet.clear();
  DT = &FAM.getResult<DominatorTreeAnalysis>(F);
  recalculateRecursionBlocks(F);
  for (BasicBlock &BB : F)
    addRecursiveBrLikely(BB);
}

/* BrLikelyInfo implementation */

/*
 * Find recursive likely branch
 *
 * @F       target function
 * @FAM     function analysis manager
 */
void BrLikelyInfo::findRecursiveLikely(Function &F,
                                       FunctionAnalysisManager &FAM) {
  RecursiveBrInfo brInfo;
  brInfo.recalculate(F, FAM);
  BrSet res = brInfo.getBrSet();
  brSet.insert(res.begin(), res.end());
}

/*
 * Find loop likely branch
 *
 * @F       target function
 * @FAM     function analysis manager
 */
void BrLikelyInfo::findLoopLikely(Function &F, FunctionAnalysisManager &FAM) {
  LoopBranch::LoopAnalysis a;
  LoopBranch::Loops loop = a.run(F, FAM);
  for (BasicBlock &BB : F)
    for (auto [br, EQ] : loop.getExitBranches(&BB)) {
      Value *cond = br->getCondition();
      brSet.insert({br, EQ});
    }
}

/*
 * Recalculate likely branches in given funcion
 *
 * @F       target function
 * @FAM     function analysis manager
 */
void BrLikelyInfo::recalculate(Function &F, FunctionAnalysisManager &FAM) {
  brSet.clear();
  findLoopLikely(F, FAM);
  findRecursiveLikely(F, FAM);
}

/*
 * Get likely branches if possible
 *
 * @br      target branch instruction
 * @return  BrLikely (br will be nullptr is not likely branch)
 */
BrLikely BrLikelyInfo::getBrLikely(BranchInst *br) {
  bool bt = brSet.find({br, true}) != brSet.end(),
       bf = brSet.find({br, false}) != brSet.end();
  if (bt && !bf)
    return {br, true};
  if (bf && !bt)
    return {br, false};
  return {nullptr, false};
}

/*
 * Print likely branches
 *
 * @OS       output stream
 */
void BrLikelyInfo::print(raw_ostream &OS) {
  for (auto &[br, cond] : brSet)
    OS << *br << " " << cond << " \n";
}

/* BrLikelyPrinterPass implementation */

BrLikelyPrinterPass::BrLikelyPrinterPass(raw_ostream &OS) : OS(OS) {}

PreservedAnalyses BrLikelyPrinterPass::run(Function &F,
                                           FunctionAnalysisManager &FAM) {
  OS << "Branch likely for function: " << F.getName() << "\n";
  BrLikelyInfo brInfo;
  brInfo.recalculate(F, FAM);
  brInfo.print(OS);
  return PreservedAnalyses::all();
}

/* pass plugin info  */

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "BrLikelyPrinterPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "BrLikelyPrinterPass") {
                    FPM.addPass(BrLikelyPrinterPass(outs()));
                    return true;
                  }
                  return false;
                });
          }};
}
