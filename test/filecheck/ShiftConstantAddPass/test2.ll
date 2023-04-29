define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     %a = call i32 @f(i32 2, i32 3)
; CHECK-NEXT:     %b = call i32 @g(i32 4, i32 5)
; CHECK-NEXT:     %add = add i32 %a, %b
; CHECK-NEXT:     ret i32 %add
entry:
  %a = call i32 @f(i32 2, i32 3)
  %b = call i32 @g(i32 4, i32 5)
  %add = add i32 %a, %b
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
