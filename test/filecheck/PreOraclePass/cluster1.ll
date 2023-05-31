; check if keeping a cluster of 3 store instructions from moving past load with only one dependency works

declare i64* @malloc(i64)

define i64 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %p = 
; CHECK-NEXT:  %q = alloca i64
; CHECK-NEXT:  %r = alloca i64
; CHECK-NEXT:  %s = alloca i64
; CHECK-NEXT:  store i64 1, i64* %p
; CHECK-NEXT:  store i64 1, i64* %q
; CHECK-NEXT:  store i64 1, i64* %r
; CHECK-NEXT:  %x = load i64, i64* %p
; CHECK-NEXT:  store i64 1, i64* %s
; CHECK-NEXT:  ret i64 0
    %p = call i64* @malloc(i64 8)
    %q = alloca i64
    %r = alloca i64
    %s = alloca i64
    store i64 1, i64* %p
    store i64 1, i64* %q
    store i64 1, i64* %r
    %x = load i64, i64* %p
    store i64 1, i64* %s
    ret i64 0
}