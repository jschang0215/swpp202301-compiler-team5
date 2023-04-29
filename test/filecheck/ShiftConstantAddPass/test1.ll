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
; CHECK-NEXT:     [[MUL1:%.*]] = mul i32 %x, 4
; CHECK-NEXT:     ret i32 [[MUL1]]
entry:
  %shl1 = shl i32 %x, 2
  ret i32 %shl1
}

define i32 @g(i32 %x, i32 %y) {
; CHECK-LABEL: @g(i32 %x, i32 %y)
; CHECK:       entry:
; CHECK-NEXT:     [[MUL2:%.*]] = mul i32 %x, 16
; CHECK-NEXT:     ret i32 [[MUL2]]
entry:
  %shl2 = shl i32 %x, 4
  ret i32 %shl2
}