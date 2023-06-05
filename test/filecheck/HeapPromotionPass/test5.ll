; Test if global variable is not optimized
; Output: No change

@p = global i32* null, align 8

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
; CHECK:  entry
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call noalias i8* @malloc(i64 noundef 32) #3
  %0 = bitcast i8* %call to i32*
  store i32* %0, i32** @p, align 8
  %1 = load i32*, i32** @p, align 8
  %2 = bitcast i32* %1 to i8*
  call void @free(i8* noundef %2) #4
  ret i32 0
}

; Function Attrs: nounwind allocsize(0)
declare noalias i8* @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(i8* noundef) #2