; two different functions

define i32 @f() {
entry:
; CHECK-LABEL: @f(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i32
; CHECK-NEXT:  %q = alloca i32
; CHECK-NEXT:  %r = alloca i32
; CHECK-NEXT:  %x = add i32 1, 1
; CHECK-NEXT:  store i32 1, i32* %p
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %r
; CHECK-NEXT:  ret i32 0
    %p = alloca i32
    %q = alloca i32
    %r = alloca i32
    store i32 1, i32* %p
    store i32 1, i32* %q
    store i32 1, i32* %r
    %x = add i32 1, 1
    ret i32 0
}

define i32 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i32
; CHECK-NEXT:  %q = alloca i32
; CHECK-NEXT:  %r = alloca i32
; CHECK-NEXT:  %x = call i32 @f()
; CHECK-NEXT:  store i32 1, i32* %p
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %r
; CHECK-NEXT:  ret i32 0
    %p = alloca i32
    %q = alloca i32
    %r = alloca i32
    store i32 1, i32* %p
    store i32 1, i32* %q
    store i32 1, i32* %r
    %x = call i32 @f()
    ret i32 0
}