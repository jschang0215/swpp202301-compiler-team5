; Test Aload reordering
; unconditional branch

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[P1:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P1]], align 4
; CHECK-NEXT:     [[P2:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P2]], align 4
; CHECK-NEXT:     [[A1:%.*]] = call i32 @aload_i32(i32* [[P1]])
; CHECK-NEXT:     [[A2:%.*]] = call i32 @aload_i32(i32* [[P2]])
; CHECK-NEXT:     br label [[END:%.*]]
; CHECK:       end:
; CHECK-NEXT:     [[C1:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C2:%.*]] = add i32 0, [[C1]]
; CHECK-NEXT:     [[U1:%.*]] = add i32 [[A1]], [[A2]]
; CHECK-NEXT:     [[U2:%.*]] = mul i32 [[U1]], [[C2]]
; CHECK-NEXT:     ret i32 [[U2]]
entry:
  %p1 = alloca i32, align 4
  store i32 0, i32* %p1, align 4
  %p2 = alloca i32, align 4
  store i32 0, i32* %p2, align 4
  br label %end
end:
  %a1 = call i32 @aload_i32(i32* %p1)
  %a2 = call i32 @aload_i32(i32* %p2)
  %c1 = add i32 0, 0
  %c2 = add i32 0, %c1
  %u1 = add i32 %a1, %a2
  %u2 = mul i32 %u1, %c2
  ret i32 %u2
}

declare i32 @aload_i32(i32*)