; check if load instruction blocks store instructions from moving

define i32 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i32
; CHECK-NEXT:  %q = alloca i32
; CHECK-NEXT:  %r = alloca i32
; CHECK-NEXT:  %s = alloca i32
; CHECK-NEXT:  store i32 1, i32* %p
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  %x = load i32, i32* %p
; CHECK-NEXT:  store i32 1, i32* %r
; CHECK-NEXT:  store i32 1, i32* %s
; CHECK-NEXT:  ret i32 0
    %p = alloca i32
    %q = alloca i32
    %r = alloca i32
    %s = alloca i32
    store i32 1, i32* %p
    store i32 1, i32* %q
    %x = load i32, i32* %p
    store i32 1, i32* %r
    store i32 1, i32* %s
    ret i32 0
}