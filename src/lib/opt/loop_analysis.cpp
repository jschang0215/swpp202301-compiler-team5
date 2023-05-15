#include "loop_analysis.h"

using namespace LoopBranch;

/* Loops implementation */

/*
 * Find simple loop by DFS
 *

 * @BBh     header block
 * @now     traveling basic block
 * @blocks  found basic blocks in loop
 * @DT      dominator tree
 */
void LoopBranch::Loops::findSimpleLoop(BasicBlock *BBh, BasicBlock *now,
                                       set<BasicBlock *> &blocks,
                                       DominatorTree &DT) {
  if (blocks.find(now) != blocks.end())
    return;
  blocks.insert(now);

  for (BasicBlock *next : predecessors(now)) // reverse traveling
    if (next != BBh && DT.dominates(BBh, next))
      findSimpleLoop(BBh, next, blocks, DT);
}

/*
 * Add simple loop to whole loop
 *
 * @BBh     header block
 * @BBt     tail block (source of back edge)
 * @DT      dominator tree
 */
void LoopBranch::Loops::addSimpleLoop(BasicBlock *BBh, BasicBlock *BBt,
                                      DominatorTree &DT) {
  set<BasicBlock *> blocks;
  findSimpleLoop(BBh, BBt, blocks, DT);
  if (loops.find(BBh) == loops.end())
    loops[BBh] = {BBh};
  loops[BBh].insert(blocks.begin(), blocks.end());
}

/*
 * Recalculate loops in given funcion
 *
 * @F       target function
 * @FAM     function analysis manager
 */
void LoopBranch::Loops::recalculate(Function &F, FunctionAnalysisManager &FAM) {
  DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
  for (auto &BBt : F) {
    inside_of[&BBt] = {};
    BranchInst *TI = dyn_cast<BranchInst>(BBt.getTerminator());
    if (!TI)
      continue;
    for (unsigned i = 0; i < TI->getNumSuccessors(); ++i) {
      BasicBlock *BBh = TI->getSuccessor(i);
      if (DT.dominates(BBh, &BBt))
        addSimpleLoop(BBh, &BBt, DT); // forall back edge
    }
  }
  for (auto &[head, loop] : loops)
    for (BasicBlock *BB : loop)
      inside_of[BB].insert(head);
}

/*
 * Find loop starting with given basic block
 *
 * @BBh     header block
 * @return  set of basic block in loop
 */
set<BasicBlock *> LoopBranch::Loops::getLoop(BasicBlock *BBh) {
  if (loops.find(BBh) == loops.end())
    return {};
  return loops[BBh];
}

/*
 * Find loops containing given basic block
 *
 * @BB      target basic block
 * @return  set of loop containing given basic block
 */
set<BasicBlock *> LoopBranch::Loops::containigLoop(BasicBlock *BB) {
  if (inside_of.find(BB) == inside_of.end())
    return {};
  return inside_of[BB];
}

/*
 * Find exit branches of loop
 *
 * @BBh       header block
 * @return    set of exit branch instruction and condition
 */
set<pair<BranchInst *, bool>>
LoopBranch::Loops::getExitBranches(BasicBlock *BBh) {
  if (loops.find(BBh) == loops.end())
    return {};
  auto loop = loops[BBh];
  set<pair<BranchInst *, bool>> res = {};

  for (BasicBlock *BB : loop) {
    BranchInst *TI = dyn_cast<BranchInst>(BB->getTerminator());
    if (!TI || !TI->isConditional())
      continue;
    for (unsigned i = 0; i < TI->getNumSuccessors(); ++i) {
      BasicBlock *BBn = TI->getSuccessor(i);
      if (loop.find(BBn) == loop.end()) {
        res.insert({TI, (bool)i});
        break;
      }
    }
  }
  return res;
}

/*
 * Print loops
 *
 * @OS       output stream
 */
void LoopBranch::Loops::print(raw_ostream &OS) {
  for (auto &[head, loop] : loops) {
    OS << head->getName() << " : ";
    for (BasicBlock *BB : loop) {
      OS << BB->getName() << " ";
    }
    OS << "\n";
  }
}

/* LoopAnalysis implementation */

Loops LoopBranch::LoopAnalysis::run(Function &F, FunctionAnalysisManager &FAM) {
  Loops loops;
  loops.recalculate(F, FAM);
  return loops;
}

AnalysisKey LoopBranch::LoopAnalysis::Key;

/* LoopsPrinter implementation */

LoopBranch::LoopsPrinterPass::LoopsPrinterPass(raw_ostream &OS) : OS(OS) {}

PreservedAnalyses
LoopBranch::LoopsPrinterPass::run(Function &F, FunctionAnalysisManager &FAM) {
  OS << "Loops for function: " << F.getName() << "\n";

  LoopBranch::LoopAnalysis a;
  LoopBranch::Loops loop = a.run(F, FAM);
  loop.print(OS);

  return PreservedAnalyses::all();
}

/* pass plugin info  */

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopsPrinterPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "LoopsPrinterPass") {
                    FPM.addPass(LoopBranch::LoopsPrinterPass(outs()));
                    return true;
                  }
                  return false;
                });
          }};
}
