; Test Aload reordering
; conditional branch

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[P1:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P1]], align 4
; CHECK-NEXT:     [[P2:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P2]], align 4
; CHECK-NEXT:     [[P3:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P3]], align 4
; CHECK-NEXT:     [[P4:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[P4]], align 4
; CHECK-NEXT:     [[COND:%.*]] = icmp eq i32 0, 0
; CHECK-NEXT:     [[A1:%.*]] = call i32 @aload_i32(i32* [[P1]])
; CHECK-NEXT:     [[A3:%.*]] = call i32 @aload_i32(i32* [[P3]])
; CHECK-NEXT:     br i1 [[COND]], label [[TRUE:%.*]], label [[FALSE:%.*]]
; CHECK:       true:
; CHECK-NEXT:     [[A2:%.*]] = call i32 @aload_i32(i32* [[P2]])
; CHECK-NEXT:     [[C1:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C2:%.*]] = add i32 0, [[C1]]
; CHECK-NEXT:     [[U1:%.*]] = add i32 [[A1]], [[A2]]
; CHECK-NEXT:     [[U2:%.*]] = add i32 [[A3]], [[C1]]
; CHECK-NEXT:     ret i32 [[U2]]
; CHECK:       false:
; CHECK-NEXT:     [[B2:%.*]] = call i32 @aload_i32(i32* [[P4]])
; CHECK-NEXT:     [[C3:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C4:%.*]] = add i32 0, [[C3]]
; CHECK-NEXT:     [[U3:%.*]] = add i32 [[A3]], [[B2]]
; CHECK-NEXT:     [[U4:%.*]] = add i32 [[A1]], [[C4]]
; CHECK-NEXT:     ret i32 [[U4]]
entry:
  %p1 = alloca i32, align 4
  store i32 0, i32* %p1, align 4
  %p2 = alloca i32, align 4
  store i32 0, i32* %p2, align 4
  %p3 = alloca i32, align 4
  store i32 0, i32* %p3, align 4
  %p4 = alloca i32, align 4
  store i32 0, i32* %p4, align 4
  %cond = icmp eq i32 0, 0
  br i1 %cond, label %true, label %false
true:
  %a1 = call i32 @aload_i32(i32* %p1)
  %a2 = call i32 @aload_i32(i32* %p2)
  %a3 = call i32 @aload_i32(i32* %p3)
  %c1 = add i32 0, 0
  %c2 = add i32 0, %c1
  %u1 = add i32 %a1, %a2
  %u2 = add i32 %a3, %c1
  ret i32 %u2
false:
  %b1 = call i32 @aload_i32(i32* %p3)
  %b2 = call i32 @aload_i32(i32* %p4)
  %b3 = call i32 @aload_i32(i32* %p1)
  %c3 = add i32 0, 0
  %c4 = add i32 0, %c3
  %u3 = add i32 %b1, %b2
  %u4 = add i32 %b3, %c4
  ret i32 %u4
}

declare i32 @aload_i32(i32*)