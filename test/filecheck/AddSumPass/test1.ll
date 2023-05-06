; Test constant add to sum
; @f tests 7 add operand converted to 1 sum with 0 padded

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     %a = call i16 @f(i16 2)
; CHECK-NEXT:     %b = sext i16 %a to i32 
; CHECK-NEXT:     ret i32 %b
entry:
  %a = call i16 @f(i16 2)
  %b = sext i16 %a to i32 
  ret i32 %b
}

define i16 @f(i16 %x) {
; CHECK-LABEL: @f(i16 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i16 @int_sum_i16(i16 %x, i16 1, i16 %x, i16 1, i16 3, i16 %x, i16 1, i16 0)
; CHECK-NEXT:     ret i16 %0
entry:
  %0 = add i16 %x, 1
  %1 = add i16 %x, 1
  %2 = add i16 %0, 3
  %3 = add i16 %2, %0
  %4 = add i16 %1, %3
  %5 = add i16 %0, %1
  ret i16 %4
}