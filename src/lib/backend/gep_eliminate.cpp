#include "gep_eliminate.h"

#include "analysis.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"

using namespace std::string_literals;

namespace {
template <typename E> class ErrorWithGEP : public Error<ErrorWithGEP<E>> {
private:
  std::string message;

public:
  ErrorWithGEP(E &&__err, const llvm::GetElementPtrInst &__gep) noexcept {
    std::string gep;
    llvm::raw_string_ostream rso(gep);
    rso << __gep;

    message = std::string(__err.what()).append("\n["s).append(gep).append("]"s);
  }

  const char *what() const noexcept { return message.c_str(); }
};

template <typename T, typename E>
T unwrapOrThrowWithGEP(Result<T, E> &&__res,
                       const llvm::GetElementPtrInst &__gep) {
  using ResType = Result<T, E>;

  if (__res.isErr()) {
    auto err = __res.inspect();
    throw ErrorWithGEP(std::move(err), __gep);
  }

  return __res.unwrap();
}
} // namespace

namespace sc::backend::gep_elim {
llvm::PreservedAnalyses
GEPEliminatePass::run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
  llvm::IntegerType *Int64Ty = llvm::Type::getInt64Ty(M.getContext());
  std::set<llvm::GetElementPtrInst *> trashBin;
  for (llvm::Function &F : M) {
    trashBin.clear();
    for (llvm::BasicBlock &BB : F)
      for (llvm::Instruction &I : BB)
        if (llvm::GetElementPtrInst *GEPI =
                llvm::dyn_cast<llvm::GetElementPtrInst>(&I)) {

          llvm::Value *ptrOp = GEPI->getPointerOperand();
          llvm::Type *curr = ptrOp->getType();
          curr = curr->getPointerElementType();

          llvm::Instruction *pti =
              llvm::CastInst::CreateBitOrPointerCast(ptrOp, Int64Ty, "", GEPI);

          std::vector<llvm::Instruction *> v;
          v.push_back(pti);
          for (auto opIt = GEPI->idx_begin(); opIt != GEPI->idx_end(); ++opIt) {
            llvm::Value *op = *opIt;
            const auto size =
                unwrapOrThrowWithGEP(analysis::tryCalculateSize(curr), *GEPI);
            llvm::Instruction *mul = llvm::BinaryOperator::CreateMul(
                op, llvm::ConstantInt::get(Int64Ty, size, true), "", GEPI);
            llvm::Instruction *add =
                llvm::BinaryOperator::CreateAdd(v.back(), mul, "", GEPI);
            v.push_back(add);
            if (curr->isArrayTy())
              curr = curr->getArrayElementType();
          }

          llvm::Instruction *itp = llvm::CastInst::CreateBitOrPointerCast(
              v.back(), I.getType(), "", GEPI);
          GEPI->replaceAllUsesWith(itp);
          trashBin.insert(GEPI);
        }
    for (llvm::GetElementPtrInst *I : trashBin)
      I->eraseFromParent();
  }
  return llvm::PreservedAnalyses::all();
}
} // namespace sc::backend::gep_elim
