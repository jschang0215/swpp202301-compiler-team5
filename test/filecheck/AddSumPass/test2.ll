; Test constant add to sum
; @f tests 16 add operand converted to 2 sum and 1 add

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     %a = call i32 @f(i32 2)
; CHECK-NEXT:     ret i32 %a
entry:
  %a = call i32 @f(i32 2)
  ret i32 %a
}

define i32 @f(i32 %x) {
; CHECK-LABEL: @f(i32 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i32 @int_sum_i32(i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1)
; CHECK-NEXT:     %1 = call i32 @int_sum_i32(i32 %0, i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1, i32 %x)
; CHECK-NEXT:     %2 = add i32 %1, 1
; CHECK-NEXT:     ret i32 %2
entry:
  %0 = add i32 %x, 1
  %1 = add i32 %0, %0
  %2 = add i32 %1, %1
  %3 = add i32 %2, %2
  ret i32 %3
}