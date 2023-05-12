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
#include <iterator>
#include <type_traits>

using namespace SwitchBr;

bool BrToSwitchPass::tryMergeBB(BasicBlock *BBb, BasicBlock *BBc,
                                BasicBlock *BBm) {
  if (!BBc || !BBb)
    return false;

  if (find(predecessors(BBm), BBb) != predecessors(BBm).end() &&
      BBm->phis().begin() != BBm->phis().end())
    return false;

  BBm->replacePhiUsesWith(BBc, BBb);
  eraseBB.insert(BBc);
  return true;
}

BasicBlock *BrToSwitchPass::makeDefaultCase(BasicBlock *BBb, BasicBlock *BBd,
                                            BasicBlock *BBc) {
  BasicBlock *BBnew = nullptr;
  if (find(predecessors(BBd), BBb) != predecessors(BBd).end() &&
      BBd->phis().begin() != BBd->phis().end()) {
    BBnew = addBridgeBB(BBb, BBd, false);
  }
  BBd->replacePhiUsesWith(BBc, BBb);
  return BBnew;
}

BasicBlock *BrToSwitchPass::addBridgeBB(BasicBlock *BBc, BasicBlock *BBm,
                                        bool eraseBBc) {
  IRBuilder<> builder(F->getContext());
  BasicBlock *bridgeBB = BasicBlock::Create(F->getContext(), BBc->getName(), F,
                                            BBc->getNextNode());
  BBm->replacePhiUsesWith(BBc, bridgeBB);

  builder.SetInsertPoint(bridgeBB);
  builder.CreateBr(BBm);
  if (eraseBBc)
    eraseBB.insert(BBc);
  return bridgeBB;
}

bool BrToSwitchPass::makeSwitch(BasicBlock *BBb, Value *V, BlockPair BBd,
                                BlockPairMap &BBps) {
  if (BBps.size() < 1)
    return false;

  BranchInst *br = dyn_cast<BranchInst>(BBb->getTerminator());
  IRBuilder<> builder(br);

  BasicBlock *defaultBr = makeDefaultCase(BBb, BBd.BBm, BBd.BBc);

  SwitchInst *S = builder.CreateSwitch(V, BBd.BBm, BBps.size() - 1);

  for (auto [OnVal, p] : BBps) {
    if (p.BBc == BBb) // default
      S->addCase(OnVal, defaultBr ? defaultBr : p.BBm);
    else if (tryMergeBB(BBb, p.BBc, p.BBm)) // able to merge
      S->addCase(OnVal, p.BBm);
    else { // need to add BridgeBB
      S->addCase(OnVal, addBridgeBB(p.BBc, p.BBm));
    }
  }
  br->eraseFromParent();
  return true;
}

BrToSwitchPass::CondInfo BrToSwitchPass::getValueCond(ICmpInst *cmp) {
  if (!cmp || !cmp->isEquality())
    return {nullptr, nullptr, false};
  Value *V1 = cmp->getOperand(0), *V2 = cmp->getOperand(1);
  bool EQ = (cmp->getPredicate() == ICmpInst::Predicate::ICMP_EQ);
  if (dyn_cast<ConstantInt>(V1))
    return {V2, dyn_cast<ConstantInt>(V1), EQ};
  else if (dyn_cast<ConstantInt>(V2))
    return {V1, dyn_cast<ConstantInt>(V2), EQ};
  return {nullptr, nullptr, false};
}

BrToSwitchPass::SwitchCaseInfo BrToSwitchPass::getSwitchCase(BasicBlock *BB,
                                                             Value *headV) {
  BranchInst *br = dyn_cast<BranchInst>(BB->getTerminator());

  if ((eraseBB.find(BB) != eraseBB.end()) || !br || br->isUnconditional() ||
      (headV && (BB->size() != 2 || !BB->getUniquePredecessor())))
    return {nullptr, nullptr, nullptr, nullptr};
  
  ICmpInst *cmp = dyn_cast<ICmpInst>(br->getCondition());
  auto [V, C, EQ] = getValueCond(cmp);

  if (!V || (headV && V != headV) || br->getSuccessor(0) == br->getSuccessor(1))
    return {nullptr, nullptr, nullptr, nullptr};

  return {V, C, br->getSuccessor(EQ ? 0 : 1), br->getSuccessor(EQ ? 1 : 0)};
}

bool BrToSwitchPass::brToSwitch(BasicBlock *BB) {
  Value *headV = nullptr;
  BasicBlock *BBi = BB, *BBii = nullptr;
  BlockPairMap BBp;
  SwitchCaseInfo info;

  for (;; BBii = BBi, BBi = info.BBn, headV = info.V) {
    info = getSwitchCase(BBi, headV); // get switch case info
    if (info.V == nullptr || BBp.find(info.C) != BBp.end())
      break; // if not in switch
    BBp[info.C] = {info.BBc, BBi};
  }
  return makeSwitch(BB, headV, {BBi, BBii}, BBp);
}

PreservedAnalyses BrToSwitchPass::run(Function &F,
                                      FunctionAnalysisManager &FAM) {
  bool changed = false;
  eraseBB.clear();
  this->F = &F;
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
