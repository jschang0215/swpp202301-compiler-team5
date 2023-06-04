; Test all cases
; Output: Change approrpate heap

@p = dso_local global i32* null, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @f(i32* noundef %p) #0 {
; CEHCK:        entry:
; CEHCK-NEXT:     %call_stack = alloca i8, i64 256, align 1
; CEHCK-NEXT:     %p.addr = alloca i32*, align 8
; CEHCK-NEXT:     %p2 = alloca i32*, align 8
; CEHCK-NEXT:     store i32* %p, i32** %p.addr, align 8
; CEHCK-NEXT:     %0 = bitcast i8* %call_stack to i32*
; CEHCK-NEXT:     store i32* %0, i32** %p2, align 8
; CEHCK-NEXT:     %1 = load i32*, i32** %p2, align 8
; CEHCK-NEXT:     %2 = bitcast i32* %1 to i8*
; CEHCK-NEXT:     ret void
entry:
  %p.addr = alloca i32*, align 8
  %p2 = alloca i32*, align 8
  store i32* %p, i32** %p.addr, align 8
  %call = call noalias i8* @malloc(i64 noundef 256) #3
  %0 = bitcast i8* %call to i32*
  store i32* %0, i32** %p2, align 8
  %1 = load i32*, i32** %p2, align 8
  %2 = bitcast i32* %1 to i8*
  call void @free(i8* noundef %2) #4
  ret void
}

; Function Attrs: nounwind allocsize(0)
declare noalias i8* @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(i8* noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
; CHECK:    entry:
entry:
  %retval = alloca i32, align 4
  %p1 = alloca i32*, align 8
  store i32 0, i32* %retval, align 4
  %call = call noalias i8* @malloc(i64 noundef 32) #3
  %0 = bitcast i8* %call to i32*
  store i32* %0, i32** @p, align 8
  %call1 = call noalias i8* @malloc(i64 noundef 128) #3
  %1 = bitcast i8* %call1 to i32*
  store i32* %1, i32** %p1, align 8
  %2 = load i32*, i32** %p1, align 8
  call void @f(i32* noundef %2)
  %3 = load i32*, i32** @p, align 8
  %4 = bitcast i32* %3 to i8*
  call void @free(i8* noundef %4) #4
  %5 = load i32*, i32** %p1, align 8
  %6 = bitcast i32* %5 to i8*
  call void @free(i8* noundef %6) #4
  ret i32 0
}