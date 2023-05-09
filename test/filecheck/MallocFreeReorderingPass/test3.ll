define i32 @main() #0 {
; CHECK:        entry:
; CHECK-NEXT:     %retval = alloca i32, align 4
; CHECK-NEXT:     %arr1 = alloca ptr, align 8
; CHECK-NEXT:     %arr2 = alloca ptr, align 8
; CHECK-NEXT:     %arr3 = alloca ptr, align 8
; CHECK-NEXT:     store i32 0, ptr %retval, align 4
; CHECK-NEXT:     call void @f()
; CHECK-NEXT:     %call1 = call noalias ptr @malloc(i64 noundef 8)
; CHECK-NEXT:     store ptr %call1, ptr %arr2, align 8
; CHECK-NEXT:     %0 = load ptr, ptr %arr2, align 8
; CHECK-NEXT:     call void @free(ptr noundef %0)
; CHECK-NEXT:     %call = call noalias ptr @malloc(i64 noundef 8)
; CHECK-NEXT:     store ptr %call, ptr %arr1, align 8
; CHECK-NEXT:     %1 = load ptr, ptr %arr1, align 8
; CHECK-NEXT:     call void @free(ptr noundef %1)
; CHECK-NEXT:     %2 = load ptr, ptr %arr1, align 8
; CHECK-NEXT:     %tobool = icmp ne ptr %2, null
; CHECK-NEXT:     br i1 %tobool, label %if.then, label %if.end
entry:
  %retval = alloca i32, align 4
  %arr1 = alloca ptr, align 8
  %arr2 = alloca ptr, align 8
  %arr3 = alloca ptr, align 8
  store i32 0, ptr %retval, align 4
  %call = call noalias ptr @malloc(i64 noundef 8) #4
  store ptr %call, ptr %arr1, align 8
  %call1 = call noalias ptr @malloc(i64 noundef 8) #4
  store ptr %call1, ptr %arr2, align 8
  call void @f()
  %0 = load ptr, ptr %arr2, align 8
  call void @free(ptr noundef %0) #5
  %1 = load ptr, ptr %arr1, align 8
  call void @free(ptr noundef %1) #5
  %2 = load ptr, ptr %arr1, align 8
  %tobool = icmp ne ptr %2, null
  br i1 %tobool, label %if.then, label %if.end

; CHECK:        if.then:                                          ; preds = %entry
; CHECK-NEXT:     call void @f()
; CHECK-NEXT:     %call2 = call noalias ptr @malloc(i64 noundef 8)
; CHECK-NEXT:     store ptr %call2, ptr %arr3, align 8
; CHECK-NEXT:     %3 = load ptr, ptr %arr3, align 8
; CHECK-NEXT:     call void @free(ptr noundef %3)
; CHECK-NEXT:     br label %if.end
if.then:                                          ; preds = %entry
  %call2 = call noalias ptr @malloc(i64 noundef 8) #4
  store ptr %call2, ptr %arr3, align 8
  call void @f()
  %3 = load ptr, ptr %arr3, align 8
  call void @free(ptr noundef %3) #5
  br label %if.end

; CHECK:        if.end:                                           ; preds = %if.then, %entry
; CHECK-NEXT:     ret i32 0
if.end:                                           ; preds = %if.then, %entry
  ret i32 0                 
}

define void @f() {
  ret void
}

; Function Attrs: nounwind allocsize(0)
declare noalias ptr @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(ptr noundef) #3