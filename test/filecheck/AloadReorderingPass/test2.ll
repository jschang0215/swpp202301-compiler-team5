; Test Aload reordering
; multiple aloads

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[P1:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P1]], align 4
; CHECK-NEXT:     [[P2:%.*]] = call noalias i32* @malloc(i32 8)
; CHECK-NEXT:     store i32 0, i32* [[P2]], align 4
; CHECK-NEXT:     [[P3:%.*]] = call noalias i32* @malloc(i32 8)
; CHECK-NEXT:     store i32 0, i32* [[P3]], align 4
; CHECK-NEXT:     [[P4:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P4]], align 4
; CHECK-NEXT:     br label [[END:%.*]]
; CHECK:       end:
; CHECK-NEXT:     [[A2:%.*]] = call i32 @aload_i32(i32* [[P2]])
; CHECK-NEXT:     [[A3:%.*]] = call i32 @aload_i32(i32* [[P3]])
; CHECK-NEXT:     [[A4:%.*]] = call i32 @aload_i32(i32* [[P4]])
; CHECK-NEXT:     [[A1:%.*]] = call i32 @aload_i32(i32* [[P1]])
; CHECK-NEXT:     [[C1:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C2:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C3:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C4:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C5:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[U1:%.*]] = add i32 [[A1]], [[A2]]
; CHECK-NEXT:     [[U2:%.*]] = add i32 [[A3]], [[A4]]
; CHECK-NEXT:     call void @free(i32* [[P2]])
; CHECK-NEXT:     call void @free(i32* [[P3]])
; CHECK-NEXT:     ret i32 0
entry:
  %p1 = alloca i32, align 4
  store i32 0, i32* %p1, align 4
  %p2 = call noalias i32* @malloc(i32 8)
  store i32 0, i32* %p2, align 4
  %p3 = call noalias i32* @malloc(i32 8)
  store i32 0, i32* %p3, align 4
  %p4 = alloca i32, align 4
  store i32 0, i32* %p4, align 4
  br label %end
end:
  %a1 = call i32 @aload_i32(i32* %p1)
  %a2 = call i32 @aload_i32(i32* %p2)
  %a3 = call i32 @aload_i32(i32* %p3)
  %a4 = call i32 @aload_i32(i32* %p4)
  %c1 = add i32 0, 0
  %c2 = add i32 0, 0
  %c3 = add i32 0, 0
  %c4 = add i32 0, 0
  %c5 = add i32 0, 0
  %u1 = add i32 %a1, %a2
  %u2 = add i32 %a3, %a4
  call void @free(i32* %p2)
  call void @free(i32* %p3)
  ret i32 0
}

declare i32 @aload_i32(i32*)
declare noalias i32* @malloc(i32)
declare void @free(i32*)