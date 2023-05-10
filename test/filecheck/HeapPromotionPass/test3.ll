; Test where malloc's size is not constant

define void @f(i32 noundef %n) #0 {
; CHECK:      entry:
entry:
  %n.addr = alloca i32, align 4
  %arr1 = alloca ptr, align 8
  store i32 %n, ptr %n.addr, align 4
  %0 = load i32, ptr %n.addr, align 4
  %conv = sext i32 %0 to i64
  %call = call noalias ptr @malloc(i64 noundef %conv) #3
  store ptr %call, ptr %arr1, align 8
  %1 = load ptr, ptr %arr1, align 8
  call void @free(ptr noundef %1) #4
  ret void
}

; Function Attrs: nounwind allocsize(0)
declare noalias ptr @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(ptr noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
; CHECK:      entry:
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  call void @f(i32 noundef 8)
  ret i32 0
}