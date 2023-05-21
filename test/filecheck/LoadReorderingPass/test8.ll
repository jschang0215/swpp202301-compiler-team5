; test instruction reordering
; complex dependency

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[L1:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[L2:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[O1:%.*]] = add i32 1, 1
; CHECK-NEXT:     [[O2:%.*]] = add i32 2, 2
; CHECK-NEXT:     [[C:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[C]], align 4
; CHECK-NEXT:     [[L3:%.*]] = load i32, i32* [[C]], align 4
; CHECK-NEXT:     [[D:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[D]], align 4
; CHECK-NEXT:     [[L4:%.*]] = load i32, i32* [[D]], align 4
; CHECK-NEXT:     [[O3:%.*]] = add i32 3, 3
; CHECK-NEXT:     [[O4:%.*]] = add i32 [[O3]], [[O2]]
; CHECK-NEXT:     [[O5:%.*]] = add i32 5, 5
; CHECK-NEXT:     [[U1:%.*]] = mul i32 [[L1]], [[L2]]
; CHECK-NEXT:     [[U2:%.*]] = mul i32 [[U1]], [[L1]]
; CHECK-NEXT:     [[U5:%.*]] = mul i32 [[L1]], [[U2]]
; CHECK-NEXT:     [[U4:%.*]] = mul i32 [[L2]], [[L4]]
; CHECK-NEXT:     [[U3:%.*]] = mul i32 [[U2]], [[L3]]
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %l1 = load i32, i32* %a, align 4
  %l2 = load i32, i32* %b, align 4
  %u1 = mul i32 %l1, %l2
  %o1 = add i32 1, 1
  %o2 = add i32 2, 2
  %u2 = mul i32 %u1, %l1
  %c = alloca i32, align 4
  store i32 0, i32* %c, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %d, align 4
  %l3 = load i32, i32* %c, align 4
  %l4 = load i32, i32* %d, align 4
  %u3 = mul i32 %u2, %l3
  %u4 = mul i32 %l2, %l4
  %u5 = mul i32 %l1, %u2
  %o3 = add i32 3, 3
  %o4 = add i32 %o3, %o2
  %o5 = add i32 5, 5
  ret i32 0
}