define void @f() {
; CHECK-LABEL: @f()
; CHECK-NEXT:     ret void
  ret void
}

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     call void @f()
; CHECK-NEXT:     ret i32 0
entry:
  call void @f()
  ret i32 0
}