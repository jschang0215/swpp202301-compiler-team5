; Test constant add to sum
; @f tests 32 add operand converted to 5 sum

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
; CHECK-NEXT:          %0 = call i32 @int_sum_i32(i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1)
; CHECK-NEXT:          %1 = call i32 @int_sum_i32(i32 %0, i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1, i32 %x)
; CHECK-NEXT:          %2 = call i32 @int_sum_i32(i32 %1, i32 1, i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1)
; CHECK-NEXT:          %3 = call i32 @int_sum_i32(i32 %2, i32 %x, i32 1, i32 %x, i32 1, i32 %x, i32 1, i32 %x)
; CHECK-NEXT:          %4 = call i32 @int_sum_i32(i32 %3, i32 1, i32 %x, i32 1, i32 0, i32 0, i32 0, i32 0)
; CHECK-NEXT:          ret i32 %4

entry:
  %0 = add i32 %x, 1
  %1 = add i32 %0, %0
  %2 = add i32 %1, %1
  %3 = add i32 %2, %2
  %4 = add i32 %3, %3
  ret i32 %4
}