; Test for malloc and free in other basic block
; Output: Should not reorder malloc and free

define i32 @main() #0 {
; CHECK:      entry:
; CHECK-NEXT:     %0 = alloca i32, align 4
; CHECK-NEXT:     %1 = alloca i32*, align 8
; CHECK-NEXT:     store i32 0, i32* %0, align 4
; CHECK-NEXT:     %2 = call noalias i8* @malloc(i64 noundef 8)
; CHECK-NEXT:     %3 = bitcast i8* %2 to i32*
; CHECK-NEXT:     store i32* %3, i32** %1, align 8
; CHECK-NEXT:     %4 = load i32*, i32** %1, align 8
; CHECK-NEXT:     %5 = icmp ne i32* %4, null
; CHECK-NEXT:     br i1 %5, label %6, label %9
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32*, align 8
  store i32 0, i32* %0, align 4

  %2 = call noalias i8* @malloc(i64 noundef 8) #3
  %3 = bitcast i8* %2 to i32*
  store i32* %3, i32** %1, align 8

  %4 = load i32*, i32** %1, align 8
  %5 = icmp ne i32* %4, null
  br i1 %5, label %6, label %9

; CHECK:   6:    
; CHECK-NEXT:     call void @f()
; CHECK-NEXT:     %7 = load i32*, i32** %1, align 8
; CHECK-NEXT:     %8 = bitcast i32* %7 to i8*
; CHECK-NEXT:     call void @free(i8* noundef %8)
; CHECK-NEXT:     br label %9
6:    
  call void @f()

  %7 = load i32*, i32** %1, align 8
  %8 = bitcast i32* %7 to i8*
  call void @free(i8* noundef %8) #3
  br label %9

; CHECK:   9:    
; CHECK-NEXT:     ret i32 0
9:                              
  ret i32 0
}

define void @f() {
  ret void
}
declare noalias i8* @malloc(i64 noundef)
declare void @free(i8* noundef)