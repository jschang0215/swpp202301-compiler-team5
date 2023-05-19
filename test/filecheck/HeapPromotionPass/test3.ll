; Test where malloced var is returned
; Output: No candidate should be found

define void @f() #0 {
; CHECK:      entry:
entry:
  %p1 = alloca ptr, align 8
  %p2 = alloca ptr, align 8
  %call = call noalias ptr @malloc(i64 noundef 8) #3
  store ptr %call, ptr %p1, align 8
  %0 = load ptr, ptr %p1, align 8
  %cmp = icmp eq ptr %0, null
  br i1 %cmp, label %if.then, label %if.else
; CHECK:      if.then:
if.then:                                          ; preds = %entry
  %call1 = call noalias ptr @malloc(i64 noundef 8) #3
  store ptr %call1, ptr %p2, align 8
  %1 = load ptr, ptr %p2, align 8
  call void @free(ptr noundef %1) #4
  br label %if.end
; CHECK:      if.else:
if.else:                                          ; preds = %entry
  %2 = load ptr, ptr %p1, align 8
  call void @free(ptr noundef %2) #4
  br label %if.end

; CHECK:      if.end:
if.end:                                           ; preds = %if.else, %if.then
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
  call void @f()
  ret i32 0
}