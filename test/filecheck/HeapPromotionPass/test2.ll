; Test where malloc is used as function argument and malloc size is not constant
; Output: No candidate should be found

define i32 @main() #0 {
; CHECK:      entry:
entry:
  %retval = alloca i32, align 4
  %arr1 = alloca i8*, align 8
  store i32 0, i32* %retval, align 4
  %call = call noalias i8* @malloc(i64 noundef 8) #3
  store i8* %call, i8** %arr1, align 8
  %0 = load i8*, i8** %arr1, align 8
  call void @f(i8* noundef %0)
  %1 = load i8*, i8** %arr1, align 8
  call void @free(i8* noundef %1) #4

  call void @g(i32 noundef 8)
  ret i32 0
}

define void @f(i8* noundef %p) #0 {
; CHECK:      entry:
entry:
  %p.addr = alloca i8*, align 8
  store i8* %p, i8** %p.addr, align 8
  ret void
}

define void @g(i32 noundef %n) #0 {
; CHECK:      entry:
entry:
  %n.addr = alloca i32, align 4
  %arr1 = alloca i8*, align 8
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32, i32* %n.addr, align 4
  %conv = sext i32 %0 to i64
  %call = call noalias i8* @malloc(i64 noundef %conv) #3
  store i8* %call, i8** %arr1, align 8
  %1 = load i8*, i8** %arr1, align 8
  call void @free(i8* noundef %1) #4
  ret void
}

; Function Attrs: nounwind allocsize(0)
declare noalias i8* @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(i8* noundef) #2