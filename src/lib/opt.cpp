#include "opt.h"
#include "./opt/passes.h"

#include "../static_error.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"

#include "opt/aload_reordering.h"
#include "opt/declare.h"
#include "print_ir.h"

using namespace std::string_literals;

namespace sc::opt {
OptInternalError::OptInternalError(const std::exception &__e) noexcept {
  message = "exception thrown from opt\n"s + __e.what();
}

Result<std::unique_ptr<llvm::Module>, OptInternalError>
optimizeIR(std::unique_ptr<llvm::Module> &&__M,
           llvm::ModuleAnalysisManager &__MAM) noexcept {
  using RetType = Result<std::unique_ptr<llvm::Module>, OptInternalError>;

  try {
    llvm::LoopPassManager LPM;
    llvm::FunctionPassManager FPM;
    llvm::CGSCCPassManager CGPM;
    llvm::ModulePassManager MPM;

    // Add loop-level opt passes below

    FPM.addPass(llvm::createFunctionToLoopPassAdaptor(std::move(LPM)));
    // Add function-level opt passes below
    FPM.addPass(SimplePass());
    FPM.addPass(AddSumPass());
    FPM.addPass(ShiftConstantAddPass());
    FPM.addPass(HeapPromotionPass());
    FPM.addPass(MallocFreeReorderingPass());
    FPM.addPass(LoadReorderingPass());
    FPM.addPass(ToAload::LoadToAloadPass());
    FPM.addPass(AloadBlockPass());
    FPM.addPass(AloadReorderingPass());
    FPM.addPass(LicmPass());
    FPM.addPass(SccpPass());
    FPM.addPass(O3Pass());
    FPM.addPass(SwitchBr::SwitchToBrPass());
    FPM.addPass(BranchLikely::LikelyBranchConditionPass());
    FPM.addPass(SwitchBr::BrToSwitchPass());

    // add new passes above this line
    // make sure preoraclepass runs just before oraclepass
    FPM.addPass(PreOraclePass()); // do not add new passes below this line

    CGPM.addPass(llvm::createCGSCCToFunctionPassAdaptor(std::move(FPM)));
    // Add CGSCC-level opt passes below

    MPM.addPass(llvm::createModuleToPostOrderCGSCCPassAdaptor(std::move(CGPM)));
    // Add module-level opt passes below
    MPM.addPass(OraclePass());
    MPM.addPass(DeclarePass());

    MPM.run(*__M, __MAM);

    sc::print_ir::printIRIfVerbose(*__M, "After optimization");
  } catch (const std::exception &e) {
    return RetType::Err(OptInternalError(e));
  }

  return RetType::Ok(std::move(__M));
}
} // namespace sc::opt
