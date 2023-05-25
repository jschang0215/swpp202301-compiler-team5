; Test Aload reordering
; not domination

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[P1:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P1]], align 4
; CHECK-NEXT:     [[P2:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P2]], align 4
; CHECK-NEXT:     [[COND:%.*]] = icmp eq i32 0, 0
; CHECK-NEXT:     [[A1:%.*]] = call i32 @aload_i32(i32* [[P1]])
; CHECK-NEXT:     br i1 [[COND]], label [[TRUE:%.*]], label [[FALSE:%.*]]
; CHECK:       true:
; CHECK-NEXT:     [[C1:%.*]] = add i32 1, 2
; CHECK-NEXT:     [[C2:%.*]] = add i32 1, 2
; CHECK-NEXT:     br label [[END:%.*]]
; CHECK:       false:
; CHECK-NEXT:     [[C3:%.*]] = add i32 1, 2
; CHECK-NEXT:     br label [[END]]
; CHECK:       end:
; CHECK-NEXT:     [[A3:%.*]] = call i32 @aload_i32(i32* [[P2]])
; CHECK-NEXT:     [[C4:%.*]] = add i32 1, 2
; CHECK-NEXT:     [[C5:%.*]] = add i32 3, 4
; CHECK-NEXT:     [[U1:%.*]] = add i32 [[A3]], [[A3]]
; CHECK-NEXT:     ret i32 0


entry:
  %p1 = alloca i32, align 4
  store i32 0, i32* %p1, align 4
  %p2 = alloca i32, align 4
  store i32 0, i32* %p2, align 4
  %cond = icmp eq i32 0, 0
  br i1 %cond, label %true, label %false
true:
  %a1 = call i32 @aload_i32(i32* %p1)
  %c1 = add i32 1, 2
  %c2 = add i32 1, 2
  br label %end
false:
  %a2 = call i32 @aload_i32(i32* %p1)
  %c3 = add i32 1, 2
  br label %end
end:
  %a3 = call i32 @aload_i32(i32* %p2)
  %c4 = add i32 1, 2
  %c5 = add i32 3, 4
  %u1 = add i32 %a3, %a3
  ret i32 0
}

declare i32 @aload_i32(i32*)