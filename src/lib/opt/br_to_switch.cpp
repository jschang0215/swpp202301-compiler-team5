#include "br_to_switch.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "loop_analysis.h"
#include <iterator>
#include <type_traits>

using namespace SwitchBr;


/*
 * add bridge basic block to resolve phi node confliction
 *
 * original CFG : base -?> cond -?> dest 
 * modified CFG : base -?> bridge -> dest
 *
 * @cond:     condition basic block
 * @dest:     destination basic block
 * @return:   bridge basic block
 */
BasicBlock *BrToSwitchPass::addBridgeBB(BasicBlock *cond, BasicBlock *dest) {
  IRBuilder<> builder(F->getContext());
  BasicBlock *bridge = BasicBlock::Create(F->getContext(), cond->getName(), F,
                                            cond->getNextNode());
  dest->replacePhiUsesWith(cond, bridge);
  builder.SetInsertPoint(bridge);
  builder.CreateBr(dest);
  return bridge;
}


/*
 * try merge base and condition baisc block
 *
 * original CFG : base -?> cond -?> dest
 * modified CFG : base -?> dest  or  base -?> new_dest -> dest
 *
 * @base:      base basic block
 * @cond:      condition basic block
 * @dest:     destination basic block
 * @return:    case dest basic block
 */
BasicBlock* BrToSwitchPass::mergeBB(BasicBlock *base, BasicBlock *cond,
                                BasicBlock *dest) {
  if (!cond || !base)
    return nullptr;

  BasicBlock * new_dest = dest;
  if (find(predecessors(dest), base) != predecessors(dest).end() &&
      dest->phis().begin() != dest->phis().end())
    new_dest = addBridgeBB(cond, dest);
  
  dest->replacePhiUsesWith(cond, base);
  if(cond != base) eraseBB.insert(cond);
  
  return new_dest;
}

/*
 * make switch instruction last of BBb
 *
 * @base:     base basic block
 * @V:        switch value
 * @defP:     pair of default basic block its conditon block
 * @BBPs:     map for value and its dested and condition block.     
 * @return:   whether switch is created
 */
bool BrToSwitchPass::makeSwitch(BasicBlock *base, Value *V, BlockPair defP,
                                BlockPairMap &BBPs) {
  BranchInst *br = dyn_cast<BranchInst>(base->getTerminator());
  if (BBPs.size() == 0 || (BBPs.size()==1 && loopBr.find(br) != loopBr.end()))
    return false;

  Instruction * cond = dyn_cast<Instruction>(br->getCondition());
  IRBuilder<> builder(br);
  SwitchInst *S = builder.CreateSwitch(V, mergeBB(base, defP.cond, defP.dest));

  for (auto [OnVal, p] : BBPs) {
    if (p.cond == base) // default
      S->addCase(OnVal, p.dest);
    else
      S->addCase(OnVal, mergeBB(base, p.cond, p.dest));
  }
  br->eraseFromParent();
  cond->eraseFromParent();
  return true;
}


/*
 * get values, constant and EQ or NE info from compare instruction
 *
 * @cmp:      target icmp instruction
 * @return:   Condition info or nullptrs if it's not desired compare instrucion
 */
BrToSwitchPass::CondInfo BrToSwitchPass::getValueCond(ICmpInst *cmp) {
  // cmp is eaulity and has unique uses
  if (!cmp || !cmp->isEquality() || cmp->getNumUses() != 1)
    return {nullptr, nullptr, false};
  
  Value *V1 = cmp->getOperand(0), *V2 = cmp->getOperand(1);
  bool EQ = (cmp->getPredicate() == ICmpInst::Predicate::ICMP_EQ);
  
  // only one operends should be ConstantInt
  if (dyn_cast<ConstantInt>(V1) && !dyn_cast<ConstantInt>(V2))
    return {V2, dyn_cast<ConstantInt>(V1), EQ};
  else if (dyn_cast<ConstantInt>(V2) && !dyn_cast<ConstantInt>(V1))
    return {V1, dyn_cast<ConstantInt>(V2), EQ};
  
  return {nullptr, nullptr, false};
}

/*
 * get switch values, dest block and EQ or NE info of BB
 *
 * @BB:       target basic block
 * @headV:    value of switch instruction, nullptr for base basic block
 * @return:   switch case info structure or nullptrs it not switch case
 */
BrToSwitchPass::SwitchCaseInfo BrToSwitchPass::getSwitchCase(BasicBlock *BB,
                                                             Value *baseV) {
  BranchInst *br = dyn_cast<BranchInst>(BB->getTerminator());

  // not added to other switch blocks, conditional br
  // if not base, it must contain 2 line and unique predecessor
  if ((eraseBB.find(BB) != eraseBB.end()) || !br || br->isUnconditional() ||
      (baseV && (BB->size() != 2 || !BB->getUniquePredecessor())))
    return {nullptr, nullptr, nullptr, nullptr};
  
  ICmpInst *cmp = dyn_cast<ICmpInst>(br->getCondition());
  auto [V, C, EQ] = getValueCond(cmp);

  // cmp should be valid, its V is same with headV, target to different successor 
  if (!V || (baseV && V != baseV) || br->getSuccessor(0) == br->getSuccessor(1))
    return {nullptr, nullptr, nullptr, nullptr};

  return {V, C, br->getSuccessor(EQ ? 0 : 1), br->getSuccessor(EQ ? 1 : 0)};
}

/*
 * change termination br start from BB into switch
 *
 * @BB:       target basic block
 * @return:   whether changed or not
 */
bool BrToSwitchPass::brToSwitch(BasicBlock *BB) {
  Value *baseV = nullptr;
  BasicBlock *BBi = BB, *BBii = nullptr;
  BlockPairMap BBPs;
  SwitchCaseInfo info;

  for (;; BBii = BBi, BBi = info.next, baseV = info.V) {
    info = getSwitchCase(BBi, baseV);
    if (info.V == nullptr || BBPs.find(info.C) != BBPs.end()) 
      break;
    BBPs[info.C] = {info.cond, BBi};
  }
  return makeSwitch(BB, baseV, {BBi, BBii}, BBPs);
}

void BrToSwitchPass::getLoopBr(Function &F, FunctionAnalysisManager &FAM){
  loopBr.clear();
  LoopBranch::LoopAnalysis a;
  LoopBranch::Loops loops = a.run(F, FAM);
  for(BasicBlock &BB : F)
    for(auto [br, cond] : loops.getExitBranches(&BB))
      loopBr.insert(br);
}

PreservedAnalyses BrToSwitchPass::run(Function &F,
                                      FunctionAnalysisManager &FAM) {
  bool changed = false;
  eraseBB.clear();
  this->F = &F;
  getLoopBr(F, FAM);
  for (BasicBlock &BB : F)
    changed |= brToSwitch(&BB);
  for (BasicBlock *BB : eraseBB)
    if (BB)
      BB->eraseFromParent();
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "BrToSwitchPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "BrToSwitchPass") {
                    FPM.addPass(BrToSwitchPass());
                    return true;
                  }
                  return false;
                });
          }};
}
