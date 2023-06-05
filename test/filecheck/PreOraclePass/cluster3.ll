; check dependency check

define i64 @f(i64* %p, i64* %r) {
entry:
; CHECK-LABEL: @f(
; CHECK:       entry:
; CHECK-NEXT:  %q = alloca i64
; CHECK-NEXT:  store i64 0, i64* %q
; CHECK-NEXT:  %a = load i64, i64* %q
; CHECK-NEXT:  store i64 0, i64* %p
; CHECK-NEXT:  %b = load i64, i64* %r
; CHECK-NEXT:  ret i64 %b
    %q = alloca i64
    store i64 0, i64* %p
    store i64 0, i64* %q
    %a = load i64, i64* %q
    %b = load i64, i64* %r
    ret i64 %b
}

define i64 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i64
; CHECK-NEXT:  %q = alloca i64
; CHECK-NEXT:  %arr = alloca [2 x i64]
; CHECK-NEXT:  %first = getelementptr [2 x i64], [2 x i64]* %arr, i64 0, i64 0
; CHECK-NEXT:  %second = getelementptr [2 x i64], [2 x i64]* %arr, i64 0, i64 1
; CHECK-NEXT:  store i64 0, i64* %first
; CHECK-NEXT:  %c = call i64 @f(i64* %p, i64* %second)
; CHECK-NEXT:  store i64 0, i64* %q
; CHECK-NEXT:  ret i64 0
    %p = alloca i64
    %q = alloca i64
    %arr = alloca [2 x i64]
    %first = getelementptr [2 x i64], [2 x i64]* %arr, i64 0, i64 0
    %second = getelementptr [2 x i64], [2 x i64]* %arr, i64 0, i64 1
    store i64 0, i64* %q
    store i64 0, i64* %first
    %c = call i64 @f(i64* %p, i64* %second)
    ret i64 0
}