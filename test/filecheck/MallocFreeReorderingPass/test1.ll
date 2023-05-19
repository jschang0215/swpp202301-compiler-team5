; Test for only mallocs and frees
; Output should reorder malloc and free

define i32 @main() #0 {
; CHECK: entry:
; CHECK-NEXT:   %retval = alloca i32, align 4
; CHECK-NEXT:   %arr1 = alloca ptr, align 8
; CHECK-NEXT:   %arr2 = alloca ptr, align 8
; CHECK-NEXT:   %arr3 = alloca ptr, align 8
; CHECK-NEXT:   store i32 0, ptr %retval, align 4
; CHECK-NEXT:   %call1 = call noalias ptr @malloc(i64 noundef 8)
; CHECK-NEXT:   store ptr %call1, ptr %arr2, align 8
; CHECK-NEXT:   %0 = load ptr, ptr %arr2, align 8
; CHECK-NEXT:   call void @f(ptr noundef %0)
; CHECK-NEXT:   %1 = load ptr, ptr %arr2, align 8
; CHECK-NEXT:   call void @free(ptr noundef %1)
; CHECK-NEXT:   %call = call noalias ptr @malloc(i64 noundef 8)
; CHECK-NEXT:   store ptr %call, ptr %arr1, align 8
; CHECK-NEXT:   %2 = load ptr, ptr %arr1, align 8
; CHECK-NEXT:   call void @f(ptr noundef %2)
; CHECK-NEXT:   %3 = load ptr, ptr %arr1, align 8
; CHECK-NEXT:   call void @free(ptr noundef %3)
; CHECK-NEXT:   %call2 = call noalias ptr @malloc(i64 noundef 8)
; CHECK-NEXT:   store ptr %call2, ptr %arr3, align 8
; CHECK-NEXT:   %4 = load ptr, ptr %arr3, align 8
; CHECK-NEXT:   call void @f(ptr noundef %4)
; CHECK-NEXT:   %5 = load ptr, ptr %arr3, align 8
; CHECK-NEXT:   call void @free(ptr noundef %5)
; CHECK-NEXT:   ret i32 0
entry:
  %retval = alloca i32, align 4
  %arr1 = alloca ptr, align 8
  %arr2 = alloca ptr, align 8
  %arr3 = alloca ptr, align 8
  store i32 0, ptr %retval, align 4
  %call = call noalias ptr @malloc(i64 noundef 8) #3
  store ptr %call, ptr %arr1, align 8
  %call1 = call noalias ptr @malloc(i64 noundef 8) #3
  store ptr %call1, ptr %arr2, align 8
  %call2 = call noalias ptr @malloc(i64 noundef 8) #3
  store ptr %call2, ptr %arr3, align 8
  %0 = load ptr, ptr %arr3, align 8
  call void @f(ptr noundef %0)
  %1 = load ptr, ptr %arr2, align 8
  call void @f(ptr noundef %1)
  %2 = load ptr, ptr %arr1, align 8
  call void @f(ptr noundef %2)
  %3 = load ptr, ptr %arr1, align 8
  call void @free(ptr noundef %3) #4
  %4 = load ptr, ptr %arr2, align 8
  call void @free(ptr noundef %4) #4
  %5 = load ptr, ptr %arr3, align 8
  call void @free(ptr noundef %5) #4
  ret i32 0 
}

define void @f(ptr noundef %p) #0 {
; CHECK: entry:
entry:
  %p.addr = alloca ptr, align 8
  store ptr %p, ptr %p.addr, align 8
  ret void
}

; Function Attrs: nounwind allocsize(0)
declare noalias ptr @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(ptr noundef) #3