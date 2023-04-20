define i32 @f() {
; CHECK-LABEL: @f()
; CHECK-NEXT:     ret i32 0
  ret i32 0
}

define i32 @oracle() {
; CHECK-LABEL: @oracle()
; CHECK-NEXT:    call i32 @f()
; CHECK-NEXT:     ret i32 0
  call i32 @f()
  ret i32 0
}

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:    call i32 @oracle()
; CHECK-NEXT:     ret i32 0
entry:
  call i32 @oracle()
  ret i32 0
}