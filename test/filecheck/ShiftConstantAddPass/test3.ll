define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     %a = call i32 @f(i32 2, i32 3)
; CHECK-NEXT:     ret i32 %a
entry:
  %a = call i32 @f(i32 2, i32 3)
  ret i32 %a
}

define i32 @f(i32 %x, i32 %y) {
; CHECK-LABEL: @f(i32 %x, i32 %y)
; CHECK:       entry:
; CHECK-NEXT:     [[mul1:%.*]] = mul i32 %x, 4
; CHECK-NEXT:     [[udiv1:%.*]] = udiv i32 %y, 8
; CHECK-NEXT:     [[udiv2:%.*]] = udiv i32 %x, 16
; CHECK-NEXT:     [[ashr1:%.*]] = ashr i32 %y, 5
; CHECK-NEXT:     [[add1:%.*]] = add i32 [[udiv1]], [[udiv2]]
; CHECK-NEXT:     [[add2:%.*]] = add i32 [[add1]], [[ashr1]]
; CHECK-NEXT:     [[add3:%.*]] = add i32 [[add2]], 32
; CHECK-NEXT:     [[add4:%.*]] = add i32 [[add3]], [[mul1]]
; CHECK-NEXT:     ret i32 %add4
entry:
  %shl1 = shl i32 %x, 2
  %lshr1 = lshr i32 %y, 3
  %lshr2 = lshr i32 %x, 4
  %ashr1 = ashr i32 %y, 5
  %ashr2 = ashr i32 1024, 5
  %add1 = add i32 %lshr1, %lshr2
  %add2 = add i32 %add1, %ashr1
  %add3 = add i32 %add2, %ashr2
  %add4 = add i32 %add3, %shl1
  ret i32 %add4
}