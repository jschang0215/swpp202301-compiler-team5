; sccp test
; constant propagation

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     ret i32 30
entry:
  %a = add i32 2, 3
  %b = add i32 3, 4
  %c = mul i32 2, 3
  %d = mul i32 3, 4
  %e = add i32 %a, %b
  %f = add i32 %c, %d
  %g = add i32 %e, %f
  ret i32 %g
}