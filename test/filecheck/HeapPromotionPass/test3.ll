; Test where malloced var is returned
; Output: No candidate should be found

define ptr @f() #0 {
; CHECK:      entry:
entry:
  %retval = alloca ptr, align 8
  %arr2 = alloca ptr, align 8
  %call = call noalias ptr @malloc(i64 noundef 8) #3
  store ptr %call, ptr %arr2, align 8
  %0 = load ptr, ptr %arr2, align 8
  %tobool = icmp ne ptr %0, null
  br i1 %tobool, label %if.then, label %if.end

; CHECK:      if.then:
if.then:                                          ; preds = %entry
  %1 = load ptr, ptr %arr2, align 8
  store ptr %1, ptr %retval, align 8
  br label %return

; CHECK:      if.end:
if.end:                                           ; preds = %entry
  %2 = load ptr, ptr %arr2, align 8
  call void @free(ptr noundef %2) #4
  store ptr null, ptr %retval, align 8
  br label %return

; CHECK:      return:
return:                                           ; preds = %if.end, %if.then
  %3 = load ptr, ptr %retval, align 8
  ret ptr %3
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