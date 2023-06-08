#include "declare.h"

PreservedAnalyses DeclarePass::run(Module &M, ModuleAnalysisManager &MAM) {
  LLVMContext &Context = M.getContext();

  // types
  std::vector<Type *> eight_i1(8, Type::getInt1Ty(Context));
  std::vector<Type *> eight_i8(8, Type::getInt8Ty(Context));
  std::vector<Type *> eight_i16(8, Type::getInt16Ty(Context));
  std::vector<Type *> eight_i32(8, Type::getInt32Ty(Context));
  std::vector<Type *> eight_i64(8, Type::getInt64Ty(Context));
  Type *Int1 = Type::getInt1Ty(Context);
  Type *IntPtr1 = Type::getInt1PtrTy(Context);
  Type *Int8 = Type::getInt8Ty(Context);
  Type *IntPtr8 = Type::getInt8PtrTy(Context);
  Type *Int16 = Type::getInt16Ty(Context);
  Type *IntPtr16 = Type::getInt16PtrTy(Context);
  Type *Int32 = Type::getInt32Ty(Context);
  Type *IntPtr32 = Type::getInt32PtrTy(Context);
  Type *Int64 = Type::getInt64Ty(Context);
  Type *IntPtr64 = Type::getInt64PtrTy(Context);

  // aload
  FunctionType *Aload8 = FunctionType::get(Int8, IntPtr8, false);
  FunctionType *Aload16 = FunctionType::get(Int16, IntPtr16, false);
  FunctionType *Aload32 = FunctionType::get(Int32, IntPtr32, false);
  FunctionType *Aload64 = FunctionType::get(Int64, IntPtr64, false);
  M.getOrInsertFunction("aload_i8", Aload8);
  M.getOrInsertFunction("aload_i16", Aload16);
  M.getOrInsertFunction("aload_i32", Aload32);
  M.getOrInsertFunction("aload_i64", Aload64);

  // incr
  FunctionType *Incr1 = FunctionType::get(Int1, Int1, false);
  FunctionType *Incr8 = FunctionType::get(Int8, Int8, false);
  FunctionType *Incr16 = FunctionType::get(Int16, Int16, false);
  FunctionType *Incr32 = FunctionType::get(Int32, Int32, false);
  FunctionType *Incr64 = FunctionType::get(Int64, Int64, false);
  M.getOrInsertFunction("incr_i1", Incr1);
  M.getOrInsertFunction("incr_i8", Incr8);
  M.getOrInsertFunction("incr_i16", Incr16);
  M.getOrInsertFunction("incr_i32", Incr32);
  M.getOrInsertFunction("incr_i64", Incr64);

  // decr
  FunctionType *Decr1 = FunctionType::get(Int1, Int1, false);
  FunctionType *Decr8 = FunctionType::get(Int8, Int8, false);
  FunctionType *Decr16 = FunctionType::get(Int16, Int16, false);
  FunctionType *Decr32 = FunctionType::get(Int32, Int32, false);
  FunctionType *Decr64 = FunctionType::get(Int64, Int64, false);
  M.getOrInsertFunction("decr_i1", Decr1);
  M.getOrInsertFunction("decr_i8", Decr8);
  M.getOrInsertFunction("decr_i16", Decr16);
  M.getOrInsertFunction("decr_i32", Decr32);
  M.getOrInsertFunction("decr_i64", Decr64);

  // sum
  FunctionType *i1_type = FunctionType::get(Int1, eight_i1, false);
  FunctionType *i8_type = FunctionType::get(Int8, eight_i8, false);
  FunctionType *i16_type = FunctionType::get(Int16, eight_i16, false);
  FunctionType *i32_type = FunctionType::get(Int32, eight_i32, false);
  FunctionType *i64_type = FunctionType::get(Int64, eight_i64, false);

  M.getOrInsertFunction("int_sum_i1", i1_type);
  M.getOrInsertFunction("int_sum_i8", i8_type);
  M.getOrInsertFunction("int_sum_i16", i16_type);
  M.getOrInsertFunction("int_sum_i32", i32_type);
  M.getOrInsertFunction("int_sum_i64", i64_type);

  return PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DeclarePass", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "DeclarePass") {
                    MPM.addPass(DeclarePass());
                    return true;
                  }
                  return false;
                });
          }};
}