; Test where malloced var is returned
; Output: No candidate should be found

define void @f() #0 {
; CHECK:      entry:
; CHECK-NEXT:     %call1_stack = alloca i8, i64 8, align 1
; CHECK-NEXT:     %call_stack = alloca i8, i64 8, align 1
; CHECK-NEXT:     %p1 = alloca i8*, align 8
; CHECK-NEXT:     %p2 = alloca i8*, align 8
; CHECK-NEXT:     store i8* %call_stack, i8** %p1, align 8
; CHECK-NEXT:     %0 = load i8*, i8** %p1, align 8
; CHECK-NEXT:     %cmp = icmp eq i8* %0, null
; CHECK-NEXT:     br i1 %cmp, label %if.then, label %if.else
; CHECK:      if.then:                                          ; preds = %entry
; CHECK-NEXT:     store i8* %call1_stack, i8** %p2, align 8
; CHECK-NEXT:     %1 = load i8*, i8** %p2, align 8
; CHECK-NEXT:     br label %if.end
; CHECK:      if.else:                                          ; preds = %entry
; CHECK-NEXT:     %2 = load i8*, i8** %p1, align 8
; CHECK-NEXT:     br label %if.end
; CHECK:      if.end:                                           ; preds = %if.else, %if.then
; CHECK-NEXT:     ret void
entry:
  %p1 = alloca i8*, align 8
  %p2 = alloca i8*, align 8
  %call = call noalias i8* @malloc(i64 noundef 8) #3
  store i8* %call, i8** %p1, align 8
  %0 = load i8*, i8** %p1, align 8
  %cmp = icmp eq i8* %0, null
  br i1 %cmp, label %if.then, label %if.else
if.then:                                          ; preds = %entry
  %call1 = call noalias i8* @malloc(i64 noundef 8) #3
  store i8* %call1, i8** %p2, align 8
  %1 = load i8*, i8** %p2, align 8
  call void @free(i8* noundef %1) #4
  br label %if.end
if.else:                                          ; preds = %entry
  %2 = load i8*, i8** %p1, align 8
  call void @free(i8* noundef %2) #4
  br label %if.end
if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: nounwind allocsize(0)
declare noalias i8* @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(i8* noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
; CHECK:      entry:
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @f()
  ret i32 0
}