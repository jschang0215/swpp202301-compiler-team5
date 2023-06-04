; Test for only one malloc and free

define i32 @main() #0 {
; CHECK:      entry:
; CHECK-NEXT:     %call_stack = alloca i8, i64 32, align 1
; CHECK-NEXT:     %retval = alloca i32, align 4
; CHECK-NEXT:     %p = alloca i32*, align 8
; CHECK-NEXT:     store i32 0, i32* %retval, align 4
; CHECK-NEXT:     %0 = bitcast i8* %call_stack to i32*
; CHECK-NEXT:     store i32* %0, i32** %p, align 8
; CHECK-NEXT:     %1 = load i32*, i32** %p, align 8
; CHECK-NEXT:     %arrayidx = getelementptr inbounds i32, i32* %1, i64 0
; CHECK-NEXT:     store i32 0, i32* %arrayidx, align 4
; CHECK-NEXT:     %2 = load i32*, i32** %p, align 8
; CHECK-NEXT:     %arrayidx1 = getelementptr inbounds i32, i32* %2, i64 1
; CHECK-NEXT:     store i32 1, i32* %arrayidx1, align 4
; CHECK-NEXT:     %3 = load i32*, i32** %p, align 8
; CHECK-NEXT:     %4 = bitcast i32* %3 to i8*
; CHECK-NEXT:     ret i32 0
entry:
  %retval = alloca i32, align 4
  %p = alloca i32*, align 8
  store i32 0, i32* %retval, align 4
  %call = call noalias i8* @malloc(i64 noundef 32) #3
  ;%call = alloca i8, i64 32, align 8
  %0 = bitcast i8* %call to i32*

  store i32* %0, i32** %p, align 8
  %1 = load i32*, i32** %p, align 8
  %arrayidx = getelementptr inbounds i32, i32* %1, i64 0

  store i32 0, i32* %arrayidx, align 4
  %2 = load i32*, i32** %p, align 8
  %arrayidx1 = getelementptr inbounds i32, i32* %2, i64 1

  store i32 1, i32* %arrayidx1, align 4
  %3 = load i32*, i32** %p, align 8
  %4 = bitcast i32* %3 to i8*
  call void @free(i8* noundef %4) #4

  ret i32 0
}

declare noalias i8* @malloc(i64 noundef) #1
declare void @free(i8* noundef) #2