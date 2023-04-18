define i32 @f(i32 %a, i32 %b) {
; CHECK-LABEL: @f(i32 %a, i32 %b)
; CHECK:       entry:
; CHECK-NEXT:     [[C:%.*]] = add i32 [[A:%.*]], [[B:%.*]]
; CHECK-NEXT:     ret i32 0
entry:
  %c = add i32 %a, %b
  ret i32 0
}