; Test constant lshr, ashr, shl to mul, div
; @f tests lshr to udiv
; @g tests ashr to ashr
; @h tests overall lshr, ashr, shl

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     %a = call i32 @f(i32 2, i32 3)
; CHECK-NEXT:     %b = call i32 @g(i32 4, i32 5)
; CHECK-NEXT:     %c = call i32 @h(i32 6, i32 9)
; CHECK-NEXT:     %add = add i32 %a, %b
; CHECK-NEXT:     %sub = sub i32 %add, %c
; CHECK-NEXT:     ret i32 %add
entry:
  %a = call i32 @f(i32 2, i32 3)
  %b = call i32 @g(i32 4, i32 5)
  %c = call i32 @h(i32 6, i32 9)
  %add = add i32 %a, %b
  %sub = sub i32 %add, %c
  ret i32 %add
}

define i32 @f(i32 %x, i32 %y) {
; CHECK-LABEL: @f(i32 %x, i32 %y)
; CHECK:       entry:
; CHECK-NEXT:     [[UDIV:%.*]] = udiv i32 %y, 8
; CHECK-NEXT:     ret i32 [[UDIV]]
entry:
  %lshr = lshr i32 %y, 3
  ret i32 %lshr
}

define i32 @g(i32 %x, i32 %y) {
; CHECK-LABEL: @g(i32 %x, i32 %y)
; CHECK:       entry:
; CHECK-NEXT:     [[ASHR:%.*]] = ashr i32 %y, 5
; CHECK-NEXT:     ret i32 [[ASHR]]
entry:
  %ashr = ashr i32 %y, 5
  ret i32 %ashr
}

define i32 @h(i32 %x, i32 %y) {
; CHECK-LABEL: @h(i32 %x, i32 %y)
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